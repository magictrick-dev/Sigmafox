#include <core/resource.h>

b32 
resource_define(resource *res, cc64 file_name)
{

    assert(res != NULL);
    assert(res->buffer != NULL);

    if (!fileio_file_exists(file_name)) return false;
    if (!fileio_file_is_file(file_name)) return false;

    res->name       = file_name;
    res->size       = fileio_file_size(file_name);
    res->buffer     = NULL;
    res->active     = false;
    res->defined    = true;

    return true;

}

void
resource_undefined(resource *res)
{

    assert(res != NULL);
    res->name = NULL;
    res->size = 0;
    res->active = false;
    res->defined = false;

    if (res->buffer != NULL) resource_release(res);


}

void
resource_reserve(resource *res, u64 reserve_size)
{

    if (res->buffer == NULL)
    {
        res->buffer = system_virtual_alloc(NULL, reserve_size);
        res->active = true;
    }

    else
    {
        printf("-- Warning, attempting to reserve a resource that is already reserved.\n");
    }

}

void
resource_release(resource *res)
{

    assert(res != NULL);
    if (res->buffer == NULL)
    {
        printf("-- Warning, attempting to release resource buffer that is already released.\n");
    }

    else
    {
        system_virtual_free(res->buffer);
        res->buffer = NULL;
        res->buffer = NULL;
        res->active = false;
    }

}


