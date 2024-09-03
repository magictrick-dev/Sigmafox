#ifndef SOURCE_CORE_POOL_H
#define SOURCE_CORE_POOL_H
#include <core/definitions.h>

// --- Memory Pool -------------------------------------------------------------
//
// The memory pool is a general allocator schema for managing non-spatial blocks
// of memory. This schema is optimized for typically large allocations in series.
// For example, if the pool expects several allocations of 4KB+, and blocks are
// 1024 bytes in width, then the pool works greater. For smaller allocations,
// the memory pool will begin to fragment much harder than a coalescing general
// allocator will.
//

typedef struct memory_pool
{
    void *buffer;
    u64 buffer_size;
    u64 block_size;
} memory_pool;

void*   memory_pool_allocate(memory_pool *pool, u64 size);
void    memory_pool_free(memory_pool *pool, void *ptr);
void    memory_pool_initialize(memory_pool *pool, u64 size, u64 block_size);

#endif
