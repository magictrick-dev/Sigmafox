#include <compiler/stringpool.h>

cc64
string_pool_string_from_handle(sh64 handle)
{
    cc64 result = handle->string;
    return result;
}

sh64
string_pool_insert(string_pool *pool, cc64 string)
{
    
    assert(pool != NULL);
    assert(pool->table.commit < pool->table.count);

    // Hash the string and its initial index.
    u64 length = strlen(string);
    u32 hash = string_pool_hashify(string, length);
    u32 index = hash % pool->table.count;

    // The loop will eventually validate, since we assert that there is at least
    // one entry in the table open.
    intern *current = pool->table.entries + index;
    while (true)
    {

        // Empty spot, we can use it.
        if (current->occupied != true)
        {
            break;
        }

        // Entry already exists and is actually the string.
        else if (strcmp(current->string, string) == 0)
        {
            return current;
        }

        index++;
        current = pool->table.entries + index;

    }

    // Ensure that the entry isn't occupied (it shouldn't).
    assert(current->occupied == false);
    assert(pool->buffer_offset + length + 1 < pool->buffer_size);

    cc64 result = pool->buffer + pool->buffer_offset;
    memory_copy_simple(pool->buffer + pool->buffer_offset, string, length + 1);
    pool->buffer_offset += (length + 1);

    current->string = result;
    current->hash = hash;
    current->occupied = true;
    return current;

}

u32     
string_pool_hashify(cc64 string, u64 length)
{

    u32 hash = 0x811C9DC5; // offset basis

    for (u64 byte_index = 0; byte_index < length; ++byte_index)
    {

        hash ^= string[byte_index];
        hash *= 0x01000193; // prime

    }

    return hash;

}

void    
string_pool_initialize(string_pool *pool, memory_arena *arena, u64 pool_size)
{

    assert(pool != NULL);

    void *buffer = memory_arena_push(arena, pool_size);

    void *table = buffer;
    u64 table_count = (pool_size / 1024);
    u64 table_size = table_count * sizeof(intern);

    void *pool_buffer = (u8*)buffer + table_size;
    u64 pool_buffer_size = pool_size - table_size;

    pool->buffer = pool_buffer;
    pool->buffer_size = pool_buffer_size;
    pool->buffer_offset = 0;
    pool->table.entries = table;
    pool->table.size = table_size;
    pool->table.count = table_count;
    pool->table.commit = 0;

    assert(pool->buffer_size + table_size == pool_size);

    for (u64 index = 0; index < table_count; ++index)
    {
        pool->table.entries[index].occupied     = false;
        pool->table.entries[index].string       = NULL;
        pool->table.entries[index].hash         = 0xFFFFFFFF;
    }

}

