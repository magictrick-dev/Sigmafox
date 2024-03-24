#ifndef SIGMAFOX_CORE_ALLOCATOR_H
#define SIGMAFOX_CORE_ALLOCATOR_H 
#include <core/primitives.h>
#include <core/debugging.h>

// --- Standard Allocator ------------------------------------------------------
//
// The following functions are the core memory allocation functions for the
// standard allocator and either directly wraps the standard-library malloc/free
// functions or emulates the behavior closely.
//
//      sigmafox_memory_alloc_buffer(size_t bytes)
//      sigmafox_memory_free(void *buffer)
//      sigmafox_memory_inspect(memory_stats *stats)
//
// The functions below are macros to the memory_alloc() function and are simply
// helpers to make writing allocations easier.
//
//      sigmafox_memory_alloc(size)
//      sigmafox_memory_alloc_type(type)
//      sigmafox_memory_alloc_array(type)
//
// 
// The memory_stats structure contains information about the allocations
// made using the above functions. The sigmafox_memory_inspect function
// returns true only if and only if all allocations and paired with the
// appropriate amount of frees.
//

struct memory_stats
{
    size_t  total_allocation_size;

    i64     total_alloc_calls;
    i64     total_alloc_frees;
};

SF_MAY_ASSERT   void*   sigmafox_memory_alloc_buffer(size_t bytes);
SF_MAY_ASSERT   void    sigmafox_memory_free(void *buffer);
                bool    sigmafox_memory_inspect(SF_OPTIONAL memory_stats *stats);

#define sigmafox_memory_alloc(size) sigmafox_memory_alloc_buffer(size)
#define sigmafox_memory_alloc_type(type) (type*)sigmafox_memory_alloc_buffer(sizeof(type))
#define sigmafox_memory_alloc_array(type, count) (type*)sigmafox_memory_alloc_buffer((count) * sizeof(type))

#endif
