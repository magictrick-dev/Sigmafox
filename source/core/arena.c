#include <core/arena.h>

void*   
memory_arena_push(memory_arena *arena, u64 size)
{
    assert(size + arena->commit <= arena->size);
    void *buffer = (u8*)arena->buffer + arena->commit;
    arena->commit += size;
    return buffer;
}

void    
memory_arena_pop(memory_arena *arena, u64 size)
{
    assert(arena->commit >= size);
    arena->commit -= size;
    return;
}

arena_state
memory_arena_cache_state(memory_arena *arena)
{
    u64 current_commit = arena->commit;
    return current_commit;
}

void    
memory_arena_restore_state(memory_arena *arena, arena_state cache)
{
    arena->commit = cache;
    return;
}

