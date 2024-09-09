#include <core/resource.h>

#define ADDITIONAL_BUFFER_SIZE 32

b32     
resource_is_loaded(resource *res)
{

    SF_ENSURE_PTR(res);
    b32 result = (res->buffer != NULL);
    return result;

}

b32     
resource_is_path_valid(resource *res)
{

    SF_ENSURE_PTR(res);
    SF_ENSURE_PTR(res->user_path);
    b32 valid = (fileio_file_exists(res->user_path) && fileio_file_is_file(res->user_path));
    return valid;

}

void
resource_initialize(resource *res, ccptr file_path)
{

    SF_ENSURE_PTR(res);

    res->user_path          = file_path;
    res->buffer_size        = 0;
    res->buffer_commit      = 0;
    res->buffer             = NULL;

    return;

}

void    
resource_load(resource *res)
{

    SF_ENSURE_PTR(res);
    SF_ENSURE_PTR(res->user_path);
    u64 file_size = fileio_file_size(res->user_path);
    u64 allocate_size = file_size + ADDITIONAL_BUFFER_SIZE;

    vptr resource_buffer = system_virtual_alloc(NULL, allocate_size);

    // Set the remainder to NULL characters. This makes it easier to cast the
    // resource as a text file. Not the fastest procedure but it doesn't need to be.
    for (u64 idx = file_size; file_size < allocate_size; ++idx)
        *((cptr)resource_buffer + idx) = '\0';
    
    res->buffer = res;
    res->buffer_size = system_virtual_buffer_size(resource_buffer); 
    res->buffer_commit = file_size;

}

void    
resource_unload(resource *res)
{

    SF_ENSURE_PTR(res);
    SF_ENSURE_PTR(res->buffer);
    system_virtual_free(res->buffer);

    res->buffer         = NULL;
    res->buffer_size    = 0;
    res->buffer_commit  = 0;

    return;

}


