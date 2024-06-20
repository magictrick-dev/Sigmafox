#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <core/utilities/allocator.h>

// --- Sigmafox General Allocator ----------------------------------------------
//
// The allocation_head sits behind a user's allocation. Therefore, the true
// allocation size of the request size + the header size. When a free is called,
// we look backwards and inspect the data there. We can ensure that the data is
// valid by checking to see if the start pointer matches the user pointer.
//
// When we free, we first zero out size and start from the head and then call free.
// This prevents stagnant pointers from being valid outside the context of the free
// should the OS continue to hold the page the memory allocation came from.
//

static size_t allocation_total_allocated    = 0;
static size_t allocation_total_freed        = 0;
static size_t allocation_alloc_calls        = 0;
static size_t allocation_free_calls         = 0;
static size_t allocation_peak_useage        = 0;
static size_t allocation_useage             = 0;

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

void*
memory_alloc(size_t request_size)
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

void
memory_free(void *user_ptr)
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

bool
memory_statistics(memory_alloc_stats *stats)
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
