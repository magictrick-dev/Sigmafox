#include <platform/system.hpp>
#include <unordered_map>
#include <unistd.h>
#include <sys/mman.h>

// NOTE(Chris): This is more or less a consequence of how I designed the API, since
//              I didn't anticipate that UNIX would require the size of the buffer
//              to unmap it. Windows would happily yeet the mapped pages without a size,
//              and it would assume that if you didn't provide it, you wanted to unmap
//              all of the pages you mapped. Either way, the unordered map is used to
//              track and store the sizes of these buffers to mimmic the Windows behavior
//              on UNIX. It's probably fine anyway, since we only use this for file I/O.
static std::unordered_map<vptr, u64> buffer_sizes; 

vptr    
system_virtual_alloc(vptr offset, u64 size)
{

    vptr buffer = mmap(offset, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED)
    {
        return nullptr;
    }

    buffer_sizes[buffer] = size;

    return buffer;

}

void    
system_virtual_free(vptr buffer)
{

    SF_ASSERT(buffer_sizes.find(buffer) != buffer_sizes.end());
    munmap(buffer, buffer_sizes[buffer]);

}

u64     
system_virtual_buffer_size(vptr buffer)
{

    if (buffer_sizes.find(buffer) == buffer_sizes.end())
    {
        return 0;
    }

    return buffer_sizes[buffer];

}

u64     
system_memory_page_size()
{

    u64 page_size = sysconf(_SC_PAGESIZE);
    return page_size;

}

u64     
system_resize_to_nearest_page_boundary(u64 size)
{

    u64 page_size = system_memory_page_size();
    u64 pages_required = size / page_size;
    u64 remainder = size % page_size;
    if (remainder > 0)
    {
        pages_required++;
    }

    return pages_required * page_size;

}
