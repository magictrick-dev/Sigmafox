#ifndef SOURCE_CORE_MEMOPS_H
#define SOURCE_CORE_MEMOPS_H
#include <core/definitions.h>

void memory_copy_simple(void *dest, const void *source, u64 size);
void memory_copy_ext(void *dest, const void *source, u64 size);

#endif
