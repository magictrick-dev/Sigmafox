#ifndef SIGMAFOX_PLATFORM_SYSTEM_H
#define SIGMAFOX_PLATFORM_SYSTEM_H

void*   system_virtual_alloc(void* offset, size_t size);
void    system_virtual_free(void* buffer);
size_t  system_virtual_buffer_size(void *buffer);
size_t  system_virtual_size_to_nearest_page(size_t size);
size_t  system_virtual_page_size();


#endif
