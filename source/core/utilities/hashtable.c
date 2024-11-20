#include <string.h>
#include <core/utilities/allocator.h>
#include <core/utilities/hashtable.h>

typedef struct hash_table_entry
{
    u32 hash;
    u32 tag;
} hash_table_entry;

static inline void*
hash_table_insert_at_hash(hash_table *table, u32 hash)
{

    u64 probe = hash % table->entries_total;

    // NOTE(Chris): We know that the table will never fill since we determine a
    //              specified load factor that indicates when a table should be
    //              resized. Therefore, it's okay to busy-spin this loop since it
    //              eventually fall out if the load-factor is correctly set up.
    b32 collision_encountered = false;
    while (true)
    {

        hash_table_entry *entry = (hash_table_entry*)((u8*)table->entries_array + 
                (table->entry_stride * probe));

        if (entry->tag == HASH_TABLE_TAG_NULL)
        {
            entry->tag = HASH_TABLE_TAG_VALID;
            entry->hash = hash;
            table->entries_current++;
            return (u8*)entry + table->entry_block_offset;
        }
        
        // NOTE(Chris): Collision search refers to the number of collisions encountered
        //              while searching for a place to put the entry, while the count
        //              refers how many entries that weren't O(1) insert time.
        table->entry_collision_search++;
        if (collision_encountered == false)
        {
            table->entry_collision_count++;
            collision_encountered = true;
        }

        probe++;
        if (probe == table->entries_total)
            probe = 0;

    }

}

static inline void*
hash_table_find_at_hash(hash_table *table, u32 hash)
{

    u64 probe = hash % table->entries_total;  
    u64 search_count = 0;
    while (search_count < table->entries_total)
    {
        
        hash_table_entry *entry = (hash_table_entry*)((u8*)table->entries_array + 
                (table->entry_stride * probe));

        if (entry->tag == HASH_TABLE_TAG_VALID && entry->hash == hash)
            return (u8*)entry + table->entry_block_offset;

        search_count++;
    }

    return NULL;

}

void
hash_table_resize(hash_table *table, u64 size)
{

    assert(table != NULL);

    // A new table will properly allocate and initialize with the parameters we
    // want; we won't need to redefine that here.
    hash_table overwritting_table;
    hash_table_create(&overwritting_table, table->entry_block_size, size, 
            table->load_factor_limit, table->hash_algorithm);
    
    // Now insert each valid entry into the new entries array.
    for (u64 index = 0; index < table->entries_total; ++index)
    {

        hash_table_entry *entry = (hash_table_entry*)((uint8_t*)table->entries_array + 
                (table->entry_stride * index));
        void *old_entry_block = (u8*)entry + table->entry_block_offset;

        if (entry->tag == HASH_TABLE_TAG_VALID)
        {
            void* new_entry_block = hash_table_insert_at_hash(&overwritting_table, entry->hash);
            memory_copy_simple(new_entry_block, old_entry_block, table->entry_block_size);
        }

    }

    free(table->entries_array);
    *table = overwritting_table;

}

void*
hash_table_insert_entry(hash_table *table, cc64 key)
{

    assert(table != NULL);
    assert(table->entries_array != NULL); // Ensures initialization.

    r32 current_load = (r32)table->entries_current / (r32)table->entries_total;
    if (current_load >= table->load_factor_limit) 
        hash_table_resize(table, table->entries_total * 2);

    u32 hash = table->hash_algorithm(key, strlen(key));
    void *entry_block = hash_table_insert_at_hash(table, hash);
    return entry_block;

}

void*       
hash_table_find_entry(hash_table *table, cc64 key)
{

    u32 hash = table->hash_algorithm(key, strlen(key));
    void *result = hash_table_find_at_hash(table, hash);

    return result;
}

void        
hash_table_create(hash_table *table, u32 entry_block_size, u32 initial_capacity,
        r32 load_factor_limit, hashing_method_fptr hash_func)
{

    assert(table != NULL);
    assert((load_factor_limit < 1.0f) && (load_factor_limit > 0.0f));

    u64 header_size = sizeof(hash_table_entry) + (sizeof(hash_table_entry) % 8);
    u64 block_size = entry_block_size + (entry_block_size % 8);
    u64 entry_size = header_size + block_size;

    table->entries_total            = initial_capacity;
    table->entries_current          = 0;
    table->entry_stride             = entry_size;
    table->entry_block_size         = block_size;
    table->entry_block_offset       = header_size;
    table->entry_collision_count    = 0;
    table->entry_collision_search   = 0;
    table->load_factor_limit        = load_factor_limit;
    table->hash_algorithm           = hash_func;

    void *hash_buffer = malloc(initial_capacity * entry_size);
    table->entries_array = hash_buffer;

    // Initialize all entries within the table.
    for (u64 index = 0; index < table->entries_total; ++index)
    {
        
        u64 data_offset = table->entry_stride * index;
        hash_table_entry *entry = (hash_table_entry*)((u8*)table->entries_array + data_offset);
        entry->hash = HASH_TABLE_TAG_NULL;
        entry->tag  = HASH_TABLE_TAG_NULL;

    }


}

void
hash_table_release(hash_table *table)
{
    assert(table != NULL);
    free(table->entries_array);
    table->entries_array = NULL;
}

// --- Hashing Algorithms ------------------------------------------------------
//
// FNV-1A : Fast, decent avalanche characteristics, simple to understand.
//

#define FNV1A_OFFSET_BASIS  0x811C9DC5
#define FNV1A_PRIME         0x01000193

u32
hash_function_fnv1a(const void *buffer, u64 buffer_size)
{

    u32 hash = FNV1A_OFFSET_BASIS;

    for (u64 byte_index = 0; byte_index < buffer_size; ++byte_index)
    {

        hash ^= *((u8*)buffer + byte_index);
        hash *= FNV1A_PRIME;

    }

    return hash;

}



