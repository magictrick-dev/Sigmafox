#include <core/definitions.h>
#include <platform/system.h>
#include <intrin.h>
#include <windows.h>

// --- System Timing Functions -------------------------------------------------
//
// Definitions for high-resolution system timing.
//

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
    return __rdtsc();
}

u64
system_cpustamp_frequency()
{
    
    static u64 cpu_frequency = 0;

    if (cpu_frequency == 0)
    {

        // Frequency interval is MH/z a second, so we scale this time down to
        // 1ms so the time it takes to get the frequency becomes unnoticeable
        // at runtime. This only needs to happen once and can be primed at the
        // start of the application.
        u64 frequency = system_timestamp_frequency() / 1000;

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

        cpu_frequency = (rd_end - rd_start) * 1000;
    }

    return cpu_frequency;

}

// --- System Virtual Allocation Functions -------------------------------------
//
// Definitions for allocating pages of virtual memory from the operating system.
//

void*
system_virtual_alloc(void* offset, u64 size)
{

    // Get the nearest page size and then allocate.
    u64 nearest_page_size = system_virtual_size_to_nearest_page(size);
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

u64
system_virtual_buffer_size(void *buffer)
{
    u64 size_actual = 0;

    MEMORY_BASIC_INFORMATION memory_information = {0};
    VirtualQuery(buffer, &memory_information, sizeof(MEMORY_BASIC_INFORMATION));

    size_actual = (u64)memory_information.RegionSize;
    assert(size_actual != 0);

    return size_actual;
}

u64  
system_virtual_size_to_nearest_page(u64 size)
{

    u64 page_granularity = system_virtual_page_size();

    // Determine the number of pages we need to allocate.
    u64 page_count = (size / page_granularity);
    if (size % page_granularity != 0)
        page_count++;

    u64 actual_allocation_size = page_count * page_granularity;
    assert(actual_allocation_size != 0);

    return actual_allocation_size;

}

u64 
system_virtual_page_size()
{

    // Cache the granularity size after the first call, it never changes.
    static u64 page_granularity = 0;
    if (page_granularity == 0)
    {
        SYSTEM_INFO system_info = {0};
        GetSystemInfo(&system_info);
        page_granularity = system_info.dwAllocationGranularity;
    }

    return page_granularity;

}

