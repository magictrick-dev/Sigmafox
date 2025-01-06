#include <windows.h>
#include <platform/system.hpp>
#include <intrin.h>

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

u64
system_timestamp()
{

    LARGE_INTEGER current_time = {0};
    QueryPerformanceCounter(&current_time);
    return (u64)current_time.QuadPart;

}

u64
system_timestamp_frequency()
{
    static u64 performance_frequency = 0;
    if (performance_frequency == 0)
    {
        LARGE_INTEGER frequency = {0};
        QueryPerformanceFrequency(&frequency);
        performance_frequency = frequency.QuadPart;
    }

    return performance_frequency;
}

r64
system_timestamp_difference_ss(u64 a, u64 b)
{

    u64 difference = b - a;
    r64 time_scale = (r64)difference / (r64)system_timestamp_frequency();
    return time_scale;

}

r64
system_timestamp_difference_ms(u64 a, u64 b)
{

    u64 difference = (b - a) * 1000;
    r64 time_scale = (r64)difference / (r64)system_timestamp_frequency();
    return time_scale;

}

r64
system_timestamp_difference_us(u64 a, u64 b)
{

    u64 difference = (b - a) * 1000000;
    r64 time_scale = (r64)difference / (r64)system_timestamp_frequency();
    return time_scale;

}

r64
system_timestamp_difference_ns(u64 a, u64 b)
{

    u64 difference = (b - a) * 1000000000;
    r64 time_scale = (r64)difference / (r64)system_timestamp_frequency();
    return time_scale;

}

u64
system_cpustamp()
{

    u64 result = __rdtsc();
    return result;

}

u64
system_cpustamp_frequency()
{
    
    static u64 cpu_frequency = 0;

    if (cpu_frequency == 0)
    {

        u64 frequency = system_timestamp_frequency() / 4;

        u64 start = system_timestamp();
        u64 end = 0;
        u64 elapsed = 0;

        u64 rd_start = system_cpustamp();
        u64 rd_end = 0;

        while (elapsed <= frequency)
        {
            end = system_timestamp();
            rd_end = system_cpustamp();
            elapsed = end - start;
        }

        cpu_frequency = (rd_end - rd_start) * 4;
    }

    return cpu_frequency;

}
