#ifndef SIGMAFOX_PLATFORM_SYSTEM_H
#define SIGMAFOX_PLATFORM_SYSTEM_H
#include <core/definitions.h>

u64 system_timestamp();
u64 system_timestamp_frequency();
r64 system_timestamp_difference_ss(u64 a, u64 b);
r64 system_timestamp_difference_ms(u64 a, u64 b);
r64 system_timestamp_difference_us(u64 a, u64 b);
r64 system_timestamp_difference_ns(u64 a, u64 b);

u64 system_cpustamp();
u64 system_cpustamp_frequency();

void*   system_virtual_alloc(void* offset, u64 size);
void    system_virtual_free(void* buffer);
u64 system_virtual_buffer_size(void *buffer);
u64 system_virtual_size_to_nearest_page(u64 size);
u64 system_virtual_page_size();


#endif
