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

typedef u64 arena_state;

void* memory_arena_push(memory_arena *arena, u64 size);
void memory_arena_pop(memory_arena *arena, u64 size);
void memory_arena_restore_state(memory_arena *arena, arena_state cache);
arena_state memory_arena_cache_state(memory_arena *arena);

#endif
