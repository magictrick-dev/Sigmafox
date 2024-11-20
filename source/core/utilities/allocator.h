#ifndef SIGMAFOX_CORE_ALLOCATOR_H
#define SIGMAFOX_CORE_ALLOCATOR_H
#include <core/definitions.h>

/*
// --- Allocator Interface -----------------------------------------------------
//
// In some situations, we would like to be able to swap how we allocate memory
// in certain contexts. These contexts can be pushed, swapping how memory is
// allocated for however long until the allocator is popped.
//
// The root allocator schema is malloc()/free() and it is the default allocator.
// 
// To push an allocator, simply define the two required functions and any user-defined
// data to accompany it. You can invoke memory_allocator_context() to get the pointer
// to this structure.
//
// For any allocations, use memory_allocate()/memory_release() to properly interface
// with the currently set allocator.
//

typedef void*   (*memory_allocate_fptr)(uint64_t size);
typedef void    (*memory_release_fptr)(void *buffer);
typedef void    (*memory_on_context_push_fptr)(void);
typedef void    (*memory_on_context_pop_fptr)(void);

typedef struct memory_allocator_context
{
    void                           *user_defined;

    memory_allocate_fptr            allocate;
    memory_release_fptr             release;
    memory_on_context_push_fptr     on_context_push;    // Optional callback.
    memory_on_context_push_fptr     on_context_pop;     // Optional callback.

    memory_allocator_context       *parent_allocator;
    memory_allocator_context       *default_allocator;
} memory_allocator_context;

externable memory_allocator_context*   memory_pop_allocator();
externable memory_allocator_context*   memory_get_current_allocator_context();
externable void                        memory_initialize_allocator_context();
externable void                        memory_push_allocator(memory_allocator_context *allocator);
externable void*                       memory_allocate(uint64_t size);
externable void                        memory_release(void *ptr);

#define memory_allocate_type(type) ((type*)memory_allocate(sizeof(type)))
#define memory_allocate_array(type, size) ((type*)memory_allocate(sizeof(type)*size))
#endif
*/

// --- Memory Arena ------------------------------------------------------------
//
// A monotonic stack allocator designed for speed and efficiency. Arenas should be
// allocated via the system's virtual allocation utility. You can use an arena as-is
// or create and set it as a context to use the memory_allocate/memory_release functions
// on the arena after setting its context.
//

#define memory_arena_push_type(arena, type) (type*)memory_arena_push(arena, sizeof(type))
#define memory_arena_push_array(arena, type, count) (type*)memory_arena_push(arena, sizeof(type)*(count))

typedef struct memory_arena
{
    void *buffer;
    uint64_t size;
    uint64_t commit;
} memory_arena;

externable void*    memory_arena_push(memory_arena *arena, u64 size);
externable void     memory_arena_pop(memory_arena *arena, u64 size);
externable u64      memory_arena_save_state(memory_arena *arena);
externable void     memory_arena_restore_state(memory_arena *arena, u64 state);

#endif
