#include <stdio.h>
#include <stdlib.h>
#include <core/utilities/allocator.h>

// --- Malloc/Free Wrapper -----------------------------------------------------
//
// The malloc/free wrapper adds tracking information to allocations.
//

static size_t allocation_total_allocated    = 0;
static size_t allocation_total_freed        = 0;
static size_t allocation_alloc_calls        = 0;
static size_t allocation_free_calls         = 0;
static size_t allocation_peak_useage        = 0;
static size_t allocation_useage             = 0;

typedef struct
memory_alloc_stats
{
    size_t memory_allocated;
    size_t memory_freed;
    size_t alloc_calls;
    size_t free_calls;
    size_t peak_useage;
    size_t current_useage;
} memory_alloc_stats;

static void* tracked_memory_alloc(size_t size);
static void  tracked_memory_free(void *ptr);
static bool  tracked_memory_statistics(memory_alloc_stats *stats);

typedef struct
allocation_head
{
    size_t  size;
    void*   start;
} allocation_head;

static inline size_t
allocator_get_allocation_size(size_t request_size)
{
    return request_size + sizeof(allocation_head);
}

static inline allocation_head*
allocator_get_allocation_head(void *user_ptr)
{
    allocation_head *head = (allocation_head*)((uint8_t*)user_ptr - sizeof(allocation_head));
    assert(head->start = user_ptr); // A check digit, if you will.
    return head;
}

static void*
tracked_memory_alloc(size_t request_size)
{

    void* user_buffer = NULL;

#   if defined(SIGMAFOX_DEBUG_BUILD)
        
    size_t actual_size = allocator_get_allocation_size(request_size);
    void *buffer = malloc(actual_size);
    
    allocation_head *head = (allocation_head*)buffer;
    head->size = actual_size;
    head->start = (uint8_t*)buffer + sizeof(allocation_head);
    user_buffer = head->start;

    allocation_useage += actual_size;
    allocation_total_allocated += actual_size;
    allocation_alloc_calls++;

    if (allocation_useage > allocation_peak_useage)
        allocation_peak_useage = allocation_useage;

#   else

    user_buffer = malloc(request_size);

#   endif

    return user_buffer;

}

static void
tracked_memory_free(void *user_ptr)
{

    void* free_ptr = user_ptr;

#   if defined(SIGMAFOX_DEBUG_BUILD)

    allocation_head *head = allocator_get_allocation_head(user_ptr);
    assert(head->start == user_ptr);
    assert(head->size != 0);

    allocation_useage -= head->size;
    allocation_total_freed += head->size;
    allocation_free_calls++;

    head->start = NULL;
    head->size = 0;

    free_ptr = head;

#   endif

    free(free_ptr);

}

static bool
tracked_memory_statistics(memory_alloc_stats *stats)
{

#   if defined(SIGMAFOX_DEBUG_BUILD)

    if (stats != NULL)
    {
        
        stats->memory_allocated = allocation_total_allocated;
        stats->memory_freed = allocation_total_freed;
        stats->alloc_calls = allocation_alloc_calls;
        stats->free_calls = allocation_free_calls;
        stats->peak_useage = allocation_peak_useage;
        stats->current_useage = allocation_useage;

    }

    // We determine the return value given that the values are correct.
    if (allocation_total_allocated == allocation_total_freed &&
        allocation_alloc_calls == allocation_free_calls)
            return true;
    else return false;

#   endif

    // Always return true in production, prevent runtime values.
    return true;

}

static void
tracked_memory_on_context_pop(void)
{
    
    memory_alloc_stats stats;
    bool matched_allocs = tracked_memory_statistics(&stats);
    printf("\n");
    printf("------------------------------------------------------------\n");
    printf("                Malloc/Free Statistics\n");
    printf("------------------------------------------------------------\n");
    printf("Total memory allocated:     %lld bytes.\n", stats.memory_allocated);
    printf("Total memory released:      %lld bytes.\n", stats.memory_freed);
    printf("Peak memory allocated:      %lld bytes.\n", stats.memory_freed);
    printf("Current memory used:        %lld bytes.\n", stats.current_useage);
    printf("Calls to malloc():          %lld.\n", stats.alloc_calls);
    printf("Calls to free():            %lld.\n", stats.free_calls);


}

// --- Allocator Interface -----------------------------------------------------
//
// The allocator interface uses the above tracked allocator as the default
// interface if not memory allocator is pushed onto the stack.
//

typedef struct allocator_stack
{
    memory_allocator_context    default_allocator;
    memory_allocator_context   *active_allocator;
} allocator_stack;

static allocator_stack allocator_stack_state;

void                 
memory_push_allocator(memory_allocator_context *allocator)
{
    
    memory_allocator_context *current_allocator = allocator_stack_state.active_allocator;
    allocator->parent_allocator = current_allocator;
    allocator->default_allocator = &allocator_stack_state.default_allocator;

    allocator_stack_state.active_allocator = allocator;
    
    if (allocator->on_context_push)
        allocator->on_context_push();

}

memory_allocator_context*
memory_pop_allocator()
{

    // NOTE(Chris): If the current allocator is null, then there is no allocator
    //              to pop from the stack, meaning it is just the default allocator.
    //              This is an error, so we will assert here.
    memory_allocator_context *current_allocator = allocator_stack_state.active_allocator;
    assert(current_allocator != NULL);

    if (current_allocator->on_context_pop)
        current_allocator->on_context_pop();

    allocator_stack_state.active_allocator = current_allocator->parent_allocator;
    return current_allocator; // In case we want to pop and *then* push again.

}

memory_allocator_context*
memory_get_current_allocator_context()
{
    
    memory_allocator_context *current_allocator = allocator_stack_state.active_allocator;
    return current_allocator;

}

void
memory_initialize_allocator_context()
{
 
    memory_push_allocator(&allocator_stack_state.default_allocator);
    allocator_stack_state.default_allocator.allocate = tracked_memory_alloc;
    allocator_stack_state.default_allocator.release = tracked_memory_free;
    allocator_stack_state.default_allocator.on_context_pop = tracked_memory_on_context_pop;

}

void*                
memory_allocate(uint64_t size)
{
    
    memory_allocator_context *current_allocator = allocator_stack_state.active_allocator;
    void *result = current_allocator->allocate(size);
    return result;

}

void                 
memory_release(void *ptr)
{

    memory_allocator_context *current_allocator = allocator_stack_state.active_allocator;
    current_allocator->release(ptr);
    return;

}

