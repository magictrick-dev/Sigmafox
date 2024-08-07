#ifndef SOURCE_CORE_ARENA_H
#define SOURCE_CORE_ARENA_H
#include <core/definitions.h>

#define memory_arena_push_type(arena, type) (type*)memory_arena_push(arena, sizeof(type))
#define memory_arena_push_array(arena, type, count) (type*)memory_arena_push(arena, sizeof(type)*(count))

typedef struct memory_arena
{
    void *buffer;
    u64 size;
    u64 commit;
} memory_arena;

void*       memory_arena_push(memory_arena *arena, u64 size);
void        memory_arena_pop(memory_arena *arena, u64 size);
void        memory_arena_partition(memory_arena *parent, memory_arena *child, u64 size);
u64         memory_arena_save(memory_arena *arena);
void        memory_arena_restore(memory_arena *arena, u64 cache);

#endif
