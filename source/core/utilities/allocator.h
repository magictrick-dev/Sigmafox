#ifndef SIGMAFOX_CORE_ALLOCATOR_H
#define SIGMAFOX_CORE_ALLOCATOR_H
#include <core/definitions.h>

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

typedef void* (*memory_allocate_fptr)(uint64_t size);
typedef void  (*memory_release_fptr)(void *buffer);

typedef struct memory_allocator
{
    void                   *user_defined;
    memory_allocate_fptr    memory_allocate;
    memory_release_fptr     memory_release;

    memory_allocator       *parent_allocator;
    memory_allocator       *default_allocator;
} memory_allocator;

void                memory_push_allocator(memory_allocator *allocator);
memory_allocator*   memory_pop_allocator();
memory_allocator*   memory_get_current_allocator_context();
void                memory_initialize_allocator_context(); // Called in main.
void*               memory_allocate(uint64_t size);
void                memory_release(void *ptr);

#define memory_allocate_type(type) ((type*)memory_allocate(sizeof(type)))
#define memory_allocate_array(type, size) ((type*)memory_allocate(sizeof(type)*size))

// --- Memory Arena ------------------------------------------------------------
//
// A monotonic stack allocator designed for speed and efficiency. Arenas should be
// allocated via the system's virtual allocation utility.
//

#define memory_arena_push_type(arena, type) (type*)memory_arena_push(arena, sizeof(type))
#define memory_arena_push_array(arena, type, count) (type*)memory_arena_push(arena, sizeof(type)*(count))

struct memory_arena
{
    void *buffer;
    size_t size;
    size_t commit;
};

inline void*    
memory_arena_push(memory_arena *arena, size_t size)
{
    assert(arena != NULL);
    assert(arena->buffer != NULL);
    assert(arena->commit + size <= arena->size);

    void *result = (uint8_t*)arena->buffer + arena->commit;
    arena->commit += size;
    return result;

}

inline void     
memory_arena_pop(memory_arena *arena, size_t size)
{

    assert(arena != NULL);
    assert(arena->commit - size == 0); // Pop and push mismatch.
    arena->commit -= size;

}

inline size_t   
memory_arena_save_state(memory_arena *arena)
{

    assert(arena != NULL);
    size_t result = arena->commit;
    return result;

}

inline void     
memory_arena_restore_state(memory_arena *arena, size_t state)
{

    assert(arena != NULL);
    arena->commit = state;
    return;

}


#endif
