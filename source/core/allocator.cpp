#include <core/allocator.h>
#include <cstdlib>

POSYINTERNAL u64 allocation_total = 0;
POSYINTERNAL u64 allocation_calls = 0;
POSYINTERNAL u64 allocation_frees = 0;

void *
smalloc_bytes(u64 size)
{
    void *allocation = malloc(size);
    POSY_ASSERT(allocation);
    allocation_total += size;
    allocation_calls++;
    return allocation;
}

void
smalloc_free(void *buffer)
{
    POSY_ASSERT(buffer != NULL);
    allocation_frees++;
    free(buffer);
}

u64
smalloc_total_allocated()
{
    return allocation_total;
}

u64
smalloc_total_calls()
{
    return allocation_calls;
}

u64
smalloc_total_frees()
{
    return allocation_frees;
}

void
smalloc_output_statistics()
{
    
    POSY_DEBUG_PRINT("Total allocation size: %llu bytes\n", allocation_total);
    POSY_DEBUG_PRINT("Allocation calls: %llu\n", allocation_calls);
    POSY_DEBUG_PRINT("Allocation frees: %llu\n", allocation_frees);

}
