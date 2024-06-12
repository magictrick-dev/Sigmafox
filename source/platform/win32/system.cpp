#include <platform/system.h>
#include <cassert>
#include <windows.h>

void*
system_virtual_alloc(void* offset, size_t size)
{

    // Get the nearest page size and then allocate.
    size_t nearest_page_size = system_virtual_size_to_nearest_page(size);
    void* memory_ptr = VirtualAlloc(offset, nearest_page_size,
            MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

    assert(memory_ptr != NULL);

    // Return the memory pointer.
    return memory_ptr;
}

void    
system_virtual_free(void* buffer)
{
    assert(buffer != NULL);
    VirtualFree(buffer, 0, MEM_RELEASE);
    return;
}

size_t  
system_virtual_buffer_size(void *buffer)
{
    size_t size_actual = 0;

    MEMORY_BASIC_INFORMATION memory_information = {};
    VirtualQuery(buffer, &memory_information, sizeof(MEMORY_BASIC_INFORMATION));

    size_actual = (size_t)memory_information.RegionSize;
    assert(size_actual != NULL);

    return size_actual;
}

size_t  
system_virtual_size_to_nearest_page(size_t size)
{

    size_t page_granularity = system_virtual_page_size();

    // Determine the number of pages we need to allocate.
    size_t page_count = (size / page_granularity);
    if (size % page_granularity != 0)
        page_count++;

    size_t actual_allocation_size = page_count * page_granularity;
    assert(actual_allocation_size != 0);

    return actual_allocation_size;

}

size_t  
system_virtual_page_size()
{

    // Cache the granularity size after the first call, it never changes.
    static size_t page_granularity = 0;
    if (page_granularity == 0)
    {
        SYSTEM_INFO system_info = {};
        GetSystemInfo(&system_info);
        page_granularity = system_info.dwAllocationGranularity;
    }

    return page_granularity;

}

