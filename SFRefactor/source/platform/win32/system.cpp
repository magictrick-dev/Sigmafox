#include <windows.h>
#include <platform/system.hpp>

vptr 
system_virtual_alloc(vptr offset, u64 size)
{

    u64 nearest_boundary = system_resize_to_nearest_page_boundary(size);
    vptr buffer = VirtualAlloc(offset, nearest_boundary, 
            MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    SF_ASSERT(buffer != NULL);

    return buffer;

}

void 
system_virtual_free(vptr buffer)
{

    SF_ENSURE_PTR(buffer);
    VirtualFree(buffer, 0, MEM_RELEASE);

}

u64
system_virtual_buffer_size(vptr buffer)
{

    u64 size_actual = 0;

    MEMORY_BASIC_INFORMATION memory_information = {0};
    VirtualQuery(buffer, &memory_information, sizeof(MEMORY_BASIC_INFORMATION));

    size_actual = (u64)memory_information.RegionSize;
    assert(size_actual != 0);

    return size_actual;

}

u64
system_resize_to_nearest_page_boundary(u64 size)
{

    u64 page_granularity = system_memory_page_size();

    // Determine the number of pages we need to allocate.
    u64 page_count = (size / page_granularity);
    if (size % page_granularity != 0) page_count++;

    u64 actual_allocation_size = page_count * page_granularity;
    return actual_allocation_size;

}

u64     
system_memory_page_size()
{
    
    static u64 page_granularity = 0;
    if (page_granularity == 0)
    {
        SYSTEM_INFO system_info = {0};
        GetSystemInfo(&system_info);
        page_granularity = system_info.dwAllocationGranularity;
    }

    return page_granularity;

}
