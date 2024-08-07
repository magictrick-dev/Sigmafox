#include <core/pool.h>

void*   
memory_pool_allocate(memory_pool *pool, u64 size)
{

    return NULL;
}

void    
memory_pool_free(memory_pool *pool, void *ptr)
{
    
    return;
}

void    
memory_pool_initialize(memory_pool *pool, u64 size, u64 block_size)
{
    
    printf("Creating a memory pool of size %llu\n", size);
    printf("    Block size: %llu\n", block_size);
    printf("    Block count: %llu\n", size/block_size);
    printf("    Block tracker: %llu\n", size/block_size/64);

}

