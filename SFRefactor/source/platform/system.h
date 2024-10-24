#ifndef SIGMAFOX_PLATFORM_SYSTEM_H
#define SIGMAFOX_PLATFORM_SYSTEM_H 
#include <definitions.h>

vptr    system_virtual_alloc(vptr offset, u64 size);
void    system_virtual_free(vptr buffer);
u64     system_memory_page_size();
u64     system_resize_to_nearest_page_boundary(u64 size);

u64     system_timestamp();
u64     system_timestamp_frequency();
r64     system_timestamp_difference_ss(u64 a, u64 b);
r64     system_timestamp_difference_ms(u64 a, u64 b);
r64     system_timestamp_difference_us(u64 a, u64 b);
r64     system_timestamp_difference_ns(u64 a, u64 b);

u64     system_cpustamp();
u64     system_cpustamp_frequency();

#endif
