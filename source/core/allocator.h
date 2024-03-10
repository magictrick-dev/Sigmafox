#ifndef POSY_CORE_ALLOCATOR_H
#define POSY_CORE_ALLOCATOR_H
#include <core/primitives.h>
#include <core/debugging.h>

// --- Standard Allocator ----------------------------------------------
//
// Acts as an intermediate wrapper for malloc/free with some minor tracking
// features built in.
//

#define     smalloc_type(type) (type*)(smalloc_bytes(sizeof(type)))
#define     smalloc_array(type, count) (type*)(smalloc_bytes(sizeof(type) * count))
#define     smalloc(size) (smalloc_bytes(size))
void *      smalloc_bytes(u64 size);
void        smalloc_free(void *buffer);
u64         smalloc_total_allocated();
u64         smalloc_total_calls();
u64         smalloc_total_frees();
void        smalloc_output_statistics();

#endif
