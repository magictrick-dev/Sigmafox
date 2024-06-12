#ifndef SIGMAFOX_CORE_ALLOCATOR_H
#define SIGMAFOX_CORE_ALLOCATOR_H

// --- Malloc/Free Wrapper -----------------------------------------------------
//
// The malloc/free wrapper adds tracking information to allocations. The main
// purpose of this is to be able to trivially determine if there are memory leaks
// somewhere in the program. The easiesy way to check is by 
//

typedef struct
memory_alloc_stats
{
    size_t memory_allocated;
    size_t memory_freed;
    size_t alloc_calls;
    size_t free_calls;
    size_t peak_useage;
    size_t current_useage;
} memory_alloc_stats;

#define     memory_alloc_type(type) (type*)memory_alloc(sizeof(type))
#define     memory_alloc_array(type, count) (type*)memory_alloc(sizeof(type) * count)

void*       memory_alloc(size_t size);
void        memory_free(void *ptr);
bool        memory_statistics(memory_alloc_stats *stats);

#endif
