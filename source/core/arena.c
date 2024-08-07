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

void
memory_arena_partition(memory_arena *parent, memory_arena *child, u64 size)
{

    assert(parent != NULL);
    assert(child != NULL);
    assert(parent->commit + size <= parent->size);

    void *child_buffer = memory_arena_push(parent, size);
    child->buffer = child_buffer;
    child->size = size;
    child->commit = 0;
    return;

}

u64
memory_arena_save(memory_arena *arena)
{
    u64 current_commit = arena->commit;
    return current_commit;
}

void    
memory_arena_restore(memory_arena *arena, u64 cache)
{
    arena->commit = cache;
    return;
}

