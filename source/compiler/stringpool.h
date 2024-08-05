#ifndef SOURCE_COMPILER_STRINGPOOL_H
#define SOURCE_COMPILER_STRINGPOOL_H
#include <core/definitions.h>
#include <core/arena.h>

// --- String Pooling ----------------------------------------------------------
//
// An interning trick to conserve space while also maintaining persistence.
// The string pool maintains a hash table of pointers to a set of strings,
// strings are hashed, given a position in the hashtable, then intern'd into
// the pool and the pointer to that pool location is set in the hashtable.
//
// Two conditions are possible where the string pool faults:
//      1.  The string pool itself has no additional bytes to spare for larger
//          strings. This occurs when there isn't sufficient space to accomodate
//          the allocation. The solution is increasing the string pool size.
//      2.  The hash table of string pointers hits capacity. This occurs when
//          there are more pool capacity than the ability to tablize the pointers.
//
//      In either case, increasing the size of the pool will solve both problems.
//      You can also adjust the divisor, which will potentially allow more string
//      pointers in the hash table or decrease at the expense of predicting the
//      average size of intern'd stirngs.
//

typedef struct string_pool_table
{
    cc64   *pointers;
    u64     capacity;
    u64     load;
} string_pool_table;

typedef struct string_pool
{
    c64 pool_buffer;
    u64 pool_size;
    u64 pool_offset;

    string_pool_table table;
} string_pool;

// string pool capacity / divisor = number of entries in hash table
#define STRING_POOL_DEFAULT_DIVISOR 1024

u64     string_pool_get_hash_table_divisor();
void    string_pool_set_hash_table_divisor(u64 divisor_value);
cc64    string_pool_insert_string(string_pool *pool, cc64 string);
cc64    string_pool_remove_string(string_pool *pool, cc64 string);
b32     string_pool_string_exists(string_pool *pool, cc64 string);
u32     string_pool_hashify(cc64 string);
void    string_pool_initialize(string_pool *pool, memory_arena *arena, u64 pool_size);

#endif
