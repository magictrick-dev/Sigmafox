#include <core/arena.h>

void
memory_arena_initialize(memory_arena *arena, void *buffer, u64 size)
{

    assert(arena != NULL);
    assert(arena->buffer == NULL);
    arena->buffer = buffer;
    arena->size = size;
    arena->commit_bottom = 0;
    arena->commit_top = 0;
    
}

void*   
memory_arena_push(memory_arena *arena, u64 size)
{

    assert(arena != NULL);
    assert(memory_arena_can_accomodate(arena, size));

    void *buffer = (u8*)arena->buffer + arena->commit_bottom;
    arena->commit_bottom += size;

    return buffer;

}

void    
memory_arena_pop(memory_arena *arena, u64 size)
{
    assert(arena != NULL);
    assert(arena->commit_bottom >= size); // Popped more bytes than it has pushed.
    arena->commit_bottom -= size;
    return;
}

void
memory_arena_partition(memory_arena *parent, memory_arena *child, u64 size)
{

    assert(parent != NULL);
    assert(child != NULL);
    assert(memory_arena_can_accomodate(parent, size));

    void *child_buffer = memory_arena_push(parent, size);
    memory_arena_initialize(child, child_buffer, size);

    return;

}

u64
memory_arena_save(memory_arena *arena)
{
    u64 current_commit = arena->commit_bottom;
    return current_commit;
}

void    
memory_arena_restore(memory_arena *arena, u64 cache)
{
    arena->commit_bottom = cache;
    return;
}

void*       
memory_arena_push_top(memory_arena *arena, u64 size)
{

    assert(arena != NULL);
    assert(memory_arena_can_accomodate(arena, size));

    arena->commit_bottom += size;
    void *buffer = (u8*)arena->buffer + (arena->size - arena->commit_top);
    
    return buffer;

}

void        
memory_arena_pop_top(memory_arena *arena, u64 size)
{

    assert(arena != NULL);
    assert(arena->commit_top >= size);
    arena->commit_top -= size;

}

void        
memory_arena_partition_top(memory_arena *parent, memory_arena *child, u64 size)
{

    assert(parent != NULL);
    assert(child != NULL);
    assert(memory_arena_can_accomodate(parent, size));

    void *child_buffer = memory_arena_push_top(parent, size);
    memory_arena_initialize(child, child_buffer, size);

    return;

}

u64         
memory_arena_save_top(memory_arena *arena)
{

    u64 current_commit = arena->commit_top;
    return current_commit;

}

void        
memory_arena_restore_top(memory_arena *arena, u64 state)
{

    arena->commit_top = state;
    return;

}

u64         
memory_arena_commit_size(memory_arena *arena)
{

    assert(arena != NULL);
    u64 result = arena->commit_bottom + arena->commit_top;
    return result;

}

u64         
memory_arena_free_size(memory_arena *arena)
{

    assert(arena != NULL);
    u64 result = arena->size - arena->commit_bottom + arena->commit_top;
    return result;

}

b32         
memory_arena_can_accomodate(memory_arena *arena, u64 size)
{

    assert(arena != NULL);
    u64 free_space = memory_arena_free_size(arena);
    b32 fits = (free_space >= size);
    return fits;

}


