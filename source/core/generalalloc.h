#ifndef SOURCE_CORE_GENERALALLOC_H
#define SOURCE_CORE_GENERALALLOC_H
#include <platform/system.h>

// --- Sigmafox General Allocator ----------------------------------------------
//
// Allows for general memory allocations using heap coalescing.
//

void*   sf_alloc(u64 size);
void    sf_free(void *loc);



#endif
