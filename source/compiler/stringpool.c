#include <compiler/stringpool.h>

static u64 string_pool_hash_divisor = STRING_POOL_DEFAULT_DIVISOR;

u64     
string_pool_get_hash_table_divisor()
{
    return string_pool_hash_divisor;
}

void    
string_pool_set_hash_table_divisor(u64 divisor_value)
{
    string_pool_hash_divisor = divisor_value;
}

cc64    
string_pool_insert_string(string_pool *pool, cc64 string)
{

    u32 hash = string_pool_hashify(string);
    u32 index = hash % pool->table.capacity;

    // Already exists
    if (pool->table.pointers[index] != NULL) return pool->table.pointers[index];



}

cc64    
string_pool_remove_string(string_pool *pool, cc64 string)
{
    
}

b32     
string_pool_string_exists(string_pool *pool, cc64 string)
{

}

u32     
string_pool_hashify(cc64 string)
{

    u32 hash = 0x811C9DC5; // offset basis
    u64 buffer_size = strlen(string);

    for (u64 byte_index = 0; byte_index < buffer_size; ++byte_index)
    {

        hash ^= string[byte_index];
        hash *= 0x01000193; // prime

    }

    return hash;

}

void    
string_pool_initialize(string_pool *pool, memory_arena *arena, u64 pool_size)
{

}

