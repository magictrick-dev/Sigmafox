#ifndef SIGMAFOX_CORE_ALLOCATOR_H
#define SIGMAFOX_CORE_ALLOCATOR_H
#include <core/definitions.h>

// --- Malloc/Free Wrapper -----------------------------------------------------
//
// The malloc/free wrapper adds tracking information to allocations. The main
// purpose of this is to be able to trivially determine if there are memory leaks
// somewhere in the program. The easiesy way to check is by 
//

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

#define     memory_alloc_type(type) (type*)memory_alloc(sizeof(type))
#define     memory_alloc_array(type, count) (type*)memory_alloc(sizeof(type) * count)

void*       memory_alloc(size_t size);
void        memory_free(void *ptr);
bool        memory_statistics(memory_alloc_stats *stats);

// --- Memory Arena ------------------------------------------------------------
//
// A monotonic allocator designed for speed and efficiency. Arenas should be
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
