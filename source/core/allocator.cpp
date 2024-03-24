#include <core/allocator.h>
#include <cstdlib>

// --- Internal Implementation Definitions -------------------------------------
//
// The following definitions are used internally are called by the API. This is
// done so that operating system specific definitions can be written.
//

SF_INTERNAL void*   impl_memory_virtual_alloc(void* offset, size_t size);
SF_INTERNAL void    impl_memory_virtual_free(void* buffer);
SF_INTERNAL size_t  impl_memory_virtual_buffer_size(void *buffer);
SF_INTERNAL size_t  impl_memory_virtual_size_to_nearest_page(size_t size);
SF_INTERNAL size_t  impl_memory_virtual_page_size();

// --- Standard Allocator ------------------------------------------------------
//
// The standard allocator uses some internal global constants to track the
// allocations and are marked statically to prevent them from being leaked
// externally to other translation units.
//

SF_PERSIST size_t   total_allocation    = 0;
SF_PERSIST i64      total_calls         = 0;  
SF_PERSIST i64      total_frees         = 0;

SF_MAY_ASSERT void*
sigmafox_memory_alloc_buffer(size_t size)
{

    SF_ASSERT(size > 0);
    void *buffer = malloc(size);

    total_allocation += size;
    total_calls++;

    SF_ASSERT(buffer != NULL);

    return (buffer);

}

SF_MAY_ASSERT void
sigmafox_memory_free(void *buffer)
{
    
    SF_ASSERT(buffer != NULL);
    free(buffer);

    total_frees++;
    return;

}

bool
sigmafox_memory_inspect(SF_OPTIONAL memory_stats *stats)
{

    if (stats != NULL)
    {
        stats->total_allocation_size    = total_allocation;
        stats->total_alloc_calls        = total_calls;
        stats->total_alloc_frees        = total_frees;
    }

    return (total_calls == total_frees);

}

// --- Push-down Arena Allocator -----------------------------------------------
//
// The push-down arena allocator relies on OS-specific virtual allocation methods
// to grab large regions of memory. 
//


SF_MAY_ASSERT void    
sigmafox_memarena_create(memory_arena *arena, size_t size)
{

    SF_ASSERT(arena != NULL);

    void *buffer = impl_memory_virtual_alloc(NULL, size);
    SF_ASSERT(buffer != NULL);
    
    size_t allocation_size = impl_memory_virtual_buffer_size(buffer);
    SF_ASSERT(allocation_size >= size);

    arena->buffer   = buffer;
    arena->size     = allocation_size;
    arena->offset   = 0;

    return;

}

SF_MAY_ASSERT void
sigmafox_memarena_release(memory_arena *arena)
{

    SF_ASSERT(arena != NULL);
    SF_ASSERT(arena->buffer != NULL);
    impl_memory_virtual_free(arena->buffer);
    return;

}

size_t  
sigmafox_memarena_remainder(memory_arena *arena)
{
    SF_ASSERT(arena != NULL);
    return arena->size - arena->offset;
}

void* 
sigmafox_memarena_push(memory_arena *arena, size_t size)
{

    SF_ASSERT(arena != NULL);

    // Ensure the allocation can be accomodated. We opt not to
    // return a NULL here because operation critical memory should
    // fail-hard, not fail softly.
    size_t remainder = sigmafox_memarena_remainder(arena);
    SF_ASSERT(remainder >= size);

    void *allocation = (u8*)arena->buffer + arena->offset;
    arena->offset += size;
    return allocation;

}

void
sigmafox_memarena_pop(memory_arena *arena, size_t size)
{

    SF_ASSERT(arena != NULL);

    if (arena->offset < size)
        arena->offset = 0;
    else
        arena->offset -= size;

    return;

}

SF_MAY_ASSERT void*   
operator new(std::size_t size, memory_arena *arena)
{

    SF_ASSERT(arena != NULL);
    
    void *region = sigmafox_memarena_push(arena, size);
    return region;

}

SF_MAY_ASSERT void*  
operator new[](std::size_t size, memory_arena *arena)
{

    SF_ASSERT(arena != NULL);
    
    void *region = sigmafox_memarena_push(arena, size);
    return region;

}

SF_MAY_ASSERT void
operator delete(void *buffer, std::size_t size) noexcept
{
    SF_ASSERT(arena != NULL);
    SF_ASSERT(buffer != NULL);

    //sigmafox_memarena_pop(arena, size);

}

SF_MAY_ASSERT void
operator delete[](void *buffer, std::size_t size) noexcept
{

    SF_ASSERT(arena != NULL);
    SF_ASSERT(buffer != NULL);

    //sigmafox_memarena_pop(arena, size);

}


SF_MAY_ASSERT void
operator delete(void *buffer, std::size_t size, memory_arena *arena) noexcept
{
    SF_ASSERT(arena != NULL);
    SF_ASSERT(buffer != NULL);

    sigmafox_memarena_pop(arena, size);

}

SF_MAY_ASSERT void
operator delete[](void *buffer, std::size_t size, memory_arena *arena) noexcept
{

    SF_ASSERT(arena != NULL);
    SF_ASSERT(buffer != NULL);

    sigmafox_memarena_pop(arena, size);

}

// --- Win32 Definitions -------------------------------------------------------
//
// The following functions define page allocation functions defined by Windows.
// Generally, these are for creating different kinds of custom allocators or
// highly specialized data structures that require more direct memory management.
//

#if defined(_WIN32)
#include <windows.h>

SF_INTERNAL void*
impl_memory_virtual_alloc(void* offset, size_t size)
{

    // Get the nearest page size and then allocate.
    size_t nearest_page_size = impl_memory_virtual_size_to_nearest_page(size);
    void* memory_ptr = VirtualAlloc(offset, nearest_page_size,
            MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

    SF_ASSERT(memory_ptr != NULL);

    // Return the memory pointer.
    return memory_ptr;
}

SF_INTERNAL void    
impl_memory_virtual_free(void* buffer)
{
    SF_ASSERT(buffer != NULL);
    VirtualFree(buffer, 0, MEM_RELEASE);
    return;
}

SF_INTERNAL size_t  
impl_memory_virtual_buffer_size(void *buffer)
{
    size_t size_actual = 0;

    MEMORY_BASIC_INFORMATION memory_information = {};
    VirtualQuery(buffer, &memory_information, sizeof(MEMORY_BASIC_INFORMATION));

    size_actual = (size_t)memory_information.RegionSize;
    SF_ASSERT(size_actual != NULL);

    return size_actual;
}

SF_INTERNAL size_t  
impl_memory_virtual_size_to_nearest_page(size_t size)
{

    size_t page_granularity = impl_memory_virtual_page_size();

    // Determine the number of pages we need to allocate.
    size_t page_count = (size / page_granularity);
    if (size % page_granularity != 0)
        page_count++;

    size_t actual_allocation_size = page_count * page_granularity;
    SF_ASSERT(actual_allocation_size != 0);

    return actual_allocation_size;

}

SF_INTERNAL size_t  
impl_memory_virtual_page_size()
{

    // Cache the granularity size after the first call, it never changes.
    SF_PERSIST size_t page_granularity = 0;
    if (page_granularity == 0)
    {
        SYSTEM_INFO system_info = {};
        GetSystemInfo(&system_info);
        page_granularity = system_info.dwAllocationGranularity;
    }

    return page_granularity;

}

#endif
