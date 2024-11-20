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


typedef struct intern
{
    cc64    string;
    u32     hash;
    b32     occupied;
} intern;

typedef intern* sh64;

typedef struct intern_table
{
    intern *entries;
    u64 size;
    u64 count;
    u64 commit;
} intern_table;

typedef struct string_pool
{

    c64 buffer;
    u64 buffer_size;
    u64 buffer_offset;

    intern_table table;

} string_pool;

cc64        string_pool_string_from_handle(sh64 handle);
u32         string_pool_hashify(cc64 string, u64 length);
sh64        string_pool_insert(string_pool *pool, cc64 string);
void        string_pool_initialize(string_pool *pool, memory_arena *arena, u64 pool_size);

#endif
