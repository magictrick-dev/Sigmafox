#include <core/allocator.h>
#include <cstdlib>

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
