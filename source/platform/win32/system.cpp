#include <core/definitions.h>
#include <platform/system.h>
#include <intrin.h>
#include <windows.h>
#include <cstdio>

// --- System Timing Functions -------------------------------------------------
//
// Definitions for high-resolution system timing.
//

size_t          
system_timestamp()
{

    LARGE_INTEGER current_time = {};
    QueryPerformanceCounter(&current_time);
    return (size_t)current_time.QuadPart;

}

size_t
system_timestamp_frequency()
{
    static size_t performance_frequency = 0;
    if (performance_frequency == 0)
    {
        LARGE_INTEGER frequency = {};
        QueryPerformanceFrequency(&frequency);
        performance_frequency = frequency.QuadPart;
    }

    return performance_frequency;
}

double
system_timestamp_difference_ss(size_t a, size_t b)
{

    size_t difference = b - a;
    double time_scale = (double)difference / (double)system_timestamp_frequency();
    return time_scale;

}

double
system_timestamp_difference_ms(size_t a, size_t b)
{

    size_t difference = (b - a) * 1000;
    double time_scale = (double)difference / (double)system_timestamp_frequency();
    return time_scale;

}

double
system_timestamp_difference_us(size_t a, size_t b)
{

    size_t difference = (b - a) * 1000000;
    double time_scale = (double)difference / (double)system_timestamp_frequency();
    return time_scale;

}

double
system_timestamp_difference_ns(size_t a, size_t b)
{

    size_t difference = (b - a) * 1000000000;
    double time_scale = (double)difference / (double)system_timestamp_frequency();
    return time_scale;

}

size_t
system_cpustamp()
{
    return __rdtsc();
}

size_t
system_cpustamp_frequency()
{
    
    static size_t cpu_frequency = 0;

    if (cpu_frequency == 0)
    {

        // Frequency interval is MH/z a second, so we scale this time down to
        // 1ms so the time it takes to get the frequency becomes unnoticeable
        // at runtime. This only needs to happen once and can be primed at the
        // start of the application.
        size_t frequency = system_timestamp_frequency() / 1000;

        size_t start = system_timestamp();
        size_t end = 0;
        size_t elapsed = 0;

        size_t rd_start = system_cpustamp();
        size_t rd_end = 0;

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

