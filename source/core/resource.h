#ifndef SOURCE_CORE_RESOURCE_H
#define SOURCE_CORE_RESOURCE_H
#include <core/definitions.h>
#include <platform/fileio.h>
#include <platform/system.h>

typedef struct resource
{
    cc64 name;

    void *buffer;
    u64 size;
    b32 active;
    b32 defined;
} resource;

b32     resource_define(resource *res, cc64 file_name);
void    resource_undefined(resource *res);
void    resource_reserve(resource *res, u64 reserve_size);
void    resource_release(resource *res);

#endif
