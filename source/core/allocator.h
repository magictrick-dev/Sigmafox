#ifndef SIGMAFOX_CORE_ALLOCATOR_H
#define SIGMAFOX_CORE_ALLOCATOR_H 
#include <core/primitives.h>
#include <core/debugging.h>
#include <new>

#define SF_BYTES(n)     ((size_t)n)
#define SF_KILOBYTES(n) ((size_t)SF_BYTES(n)     * 1024)
#define SF_MEGABYTES(n) ((size_t)SF_KILOBYTES(n) * 1024)
#define SF_GIGABYTES(n) ((size_t)SF_MEGABYTES(n) * 1024)
#define SF_TERABYTES(n) ((size_t)SF_GIGABYTES(n) * 1024)

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

// --- Push-down Arena Allocator -----------------------------------------------
//
// A simple monotonic allocator design for rapid and efficient memory
// management. Typically used for anything that requests more advanced
// datastructures that ordinarily are difficult to track.
//
// When creating a memory arena, you should request a significant chunk of
// memory at the start, since calls to OS-specific virtual allocation functions
// are somewhat expensive. Consider allocations over 4MB if not greater to make
// this as efficient as possible.
//
// Additionally, we define the new operator which allows it to allocate using
// the memory_arena pd-allocator as an argument. Since the pd-allocator uses
// raw memory buffers, some special setup must be designed in order to make
// this work in context.
//

struct memory_arena
{
    void   *buffer;
    size_t  size;
    size_t  offset;
};

SF_MAY_ASSERT   void    sigmafox_memarena_create(memory_arena *arena, size_t size);
SF_MAY_ASSERT   void    sigmafox_memarena_release(memory_arena *arena);
SF_MAY_ASSERT   size_t  sigmafox_memarena_remainder(memory_arena *arena);
SF_MAY_ASSERT   void*   sigmafox_memarena_push(memory_arena *arena, size_t size);
SF_MAY_ASSERT   void    sigmafox_memarena_pop(memory_arena *arena, size_t size);
#define                 sigmafox_memarena_push_type(arena, type) (type*)sigmafox_memarena_push(arena, sizeof(type))
#define                 sigmafox_memarena_push_array(arena, type, count) (type*)sigmafox_memarena_push(arena, (count) * sizeof(type))

SF_MAY_ASSERT   void*   operator new(std::size_t size, memory_arena *arena);
SF_MAY_ASSERT   void*   operator new[](std::size_t size, memory_arena *arena);
SF_MAY_ASSERT   void    operator delete(void *buffer, memory_arena *arena) noexcept;
SF_MAY_ASSERT   void    operator delete[](void *buffer, memory_arena *arena) noexcept;
SF_MAY_ASSERT   void    operator delete(void *buffer, std::size_t size, memory_arena *arena) noexcept;
SF_MAY_ASSERT   void    operator delete[](void *buffer, std::size_t size, memory_arena *arena) noexcept;

#endif
