#ifndef SIGMAFOX_CORE_UTILITIES_HASHTABLE_H
#define SIGMAFOX_CORE_UTILITIES_HASHTABLE_H
#include <core/definitions.h>

// --- Hash Table --------------------------------------------------------------
//
// A hashing table generates an array of "buckets" to which things can be stored
// and quickly accessed using c-strings. Creating a hash table requires some
// structure to be initialized with a particular hashing function. The type of
// hashing function is largerly irrelevant for functionality. FNV-1A is a popular
// choice for its speed and low collision rate.
//
// Entries in the hash table are laid out continuously, but not contiguously. This
// means that the actual location in data may be offset slightly from the actual
// head of the entry array and requires some basic pointer arithmetic to actually
// get to the data pointer.
//
//      - Block sizes can be of arbitrary fixed sizes. This means anything can be
//        a value for any key/value pair.
//      - Data continuity allows for better caching.
//      - Memory alignment helps for rehashing when the load factor exceeds the threshold.
// 


#define HASH_TABLE_TAG_NULL         0xFFFFFFFF
#define HASH_TABLE_TAG_VALID        0xDEADBEEF
#define HASH_TABLE_DEFAULT_LF       0.75f

typedef uint32_t (*hashing_method_fptr)(const void *buffer, uint64_t buffer_size);
typedef struct hash_table
{
    void       *entries_array;
    uint32_t    entries_total;
    uint32_t    entries_current;
    uint32_t    entry_stride;
    uint32_t    entry_block_offset;
    uint32_t    entry_block_size;
    uint32_t    entry_collision_count;
    uint32_t    entry_collision_search;
    float       load_factor_limit;

    hashing_method_fptr hash_algorithm;
} hash_table;

#define     hash_table_insert_type(table, key, type) (type*)hash_table_insert_entry(table, key)
#define     hash_table_find_type(table, key, type) (type*)hash_table_find_entry(table, key)

void*       hash_table_insert_entry(hash_table *table, const char *key);
void*       hash_table_find_entry(hash_table *table, const char *key);
//void        hash_table_remove_entry(hash_table *table, char *key);
//void*       hash_table_iterate_entries(hash_table *table, void *next);
void        hash_table_resize(hash_table *table, uint64_t size);
void        hash_table_release(hash_table *table);
void        hash_table_create(hash_table *table, uint32_t entry_block_size, uint32_t initial_capacity,
            float load_factor_limit, hashing_method_fptr hash_func);

// --- Hashing Algorithms ------------------------------------------------------
//
// Below are a list of hashing algorithms that can be used with the hashtable.
//

uint32_t    hash_function_fnv1a(const void *buffer, uint64_t buffer_size);

#endif
