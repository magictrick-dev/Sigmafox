#ifndef SOURCE_CORE_RESOURCE_H
#define SOURCE_CORE_RESOURCE_H
#include <core/definitions.h>
#include <platform/fileio.h>
#include <platform/system.h>

typedef struct resource
{

    cc64    user_path;
    u64     buffer_size;
    u64     buffer_commit;
    void   *buffer;

} resource;

b32     resource_is_loaded(resource *res);
b32     resource_is_path_valid(resource *res);
void    resource_initialize(resource *res, ccptr file_path);
void    resource_load(resource *res);
void    resource_unload(resource *res);

#endif
