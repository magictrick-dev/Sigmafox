#ifndef SIGMAFOX_PLATFORM_SYSTEM_H
#define SIGMAFOX_PLATFORM_SYSTEM_H 
#include <definitions.hpp>

vptr    system_virtual_alloc(vptr offset, u64 size);
void    system_virtual_free(vptr buffer);
u64     system_virtual_buffer_size(vptr buffer);
u64     system_memory_page_size();
u64     system_resize_to_nearest_page_boundary(u64 size);

#endif
