#include <utilities/memoryalloc.hpp>
#include <platform/system.hpp>

// --- Allocator Interface -----------------------------------------------------
//
// Allocator interface definition.
//

IAllocator::
IAllocator()
{

    return;

}

IAllocator::
~IAllocator()
{

    return;

}

u64 IAllocator::
get_total_allocated() const
{
    return this->statistics.total_allocated;
}

u64 IAllocator::
get_total_released() const
{
    return this->statistics.total_released;
}

u64 IAllocator::
get_current_allocated() const
{
    return this->statistics.current_allocated;
}

u64 IAllocator::
get_peak_allocated() const
{
    return this->statistics.peak_allocated;
}

AllocatorStatistics IAllocator::
get_global_statistics()
{
    return IAllocator::global_statistics;
}

void IAllocator::
update_allocation_statistics(u64 size)
{

    // Local statistics.
    this->statistics.total_allocated += size;
    this->statistics.current_allocated += size;
    if (this->statistics.current_allocated > this->statistics.peak_allocated)
    {
        this->statistics.peak_allocated = this->statistics.current_allocated;
    }

    // Global statistics.
    IAllocator::global_statistics.total_allocated += size;
    IAllocator::global_statistics.current_allocated += size;
    if (IAllocator::global_statistics.current_allocated > IAllocator::global_statistics.peak_allocated)
    {
        IAllocator::global_statistics.peak_allocated = IAllocator::global_statistics.current_allocated;
    }

}

void IAllocator::
update_release_statistics(u64 size)
{

    // Local statistics.
    this->statistics.total_released += size;
    this->statistics.current_allocated -= size;

    // Global statistics.
    IAllocator::global_statistics.total_released += size;
    IAllocator::global_statistics.current_allocated -= size;

}


// --- Default Allocator -------------------------------------------------------
//
// The implementation for the default allocator. We need to create a header for
// the allocation in order to determine how large the allocation is.
//

struct DefaultAllocHeader
{
    u64 size;
};

void* DefaultAllocator::
allocate(u64 size)
{

    vptr ptr = malloc(size + sizeof(DefaultAllocHeader));
    if (ptr == nullptr)
    {
        return nullptr;
    }

    DefaultAllocHeader* header = (DefaultAllocHeader*)ptr;
    header->size = size;

    this->update_allocation_statistics(size + sizeof(DefaultAllocHeader));

    return (void*)(header + 1);

}

void DefaultAllocator::
release(void* ptr)
{

    SF_ENSURE_PTR(ptr != nullptr);
    if (ptr == nullptr)
    {
        return;
    }

    DefaultAllocHeader* header = (DefaultAllocHeader*)((u8*)ptr - (sizeof(DefaultAllocHeader)));
    u64 size = header->size;

    this->update_release_statistics(size + sizeof(DefaultAllocHeader));

    free(header);

}

// --- Memory Arena ------------------------------------------------------------
//
// The implementation of a memory arena.
//

MemoryArena::
MemoryArena(u64 size)
{

    this->buffer = system_virtual_alloc(NULL, size);
    this->buffer_size = system_virtual_buffer_size(this->buffer);
    this->offset = 0;


}

MemoryArena::
~MemoryArena()
{

    system_virtual_free(this->buffer);

    this->buffer = nullptr;
    this->buffer_size = 0;
    this->offset = 0;

}

void* MemoryArena::
allocate(u64 size)
{

    // These are stubs since you don't actually use allocate/release.
    SF_ASSERT(!"You are most likely using this interface incorrectly.");
    return nullptr;

}

void MemoryArena::
release(void* ptr)
{

    // These are stubs since you don't actually use allocate/release.
    SF_ASSERT(!"You are most likely using this interface incorrectly.");
    return;

}

vptr MemoryArena::
push(u64 size)
{

    SF_ASSERT(this->offset + size <= this->buffer_size);
    vptr ptr = (vptr)((u8*)this->buffer + this->offset);
    this->offset += size;
    this->update_allocation_statistics(size);

    return ptr;

}

void MemoryArena::
pop(u64 size)
{

    this->update_release_statistics(size);

    SF_ASSERT(this->offset >= size);

    this->offset -= size;

}

template <typename T> T* MemoryArena::
push_type()
{

    return new ((T*)this->push(sizeof(T))) T();

}

template <typename T, class... Args> T* MemoryArena::
push_type(Args... args)
{

    return new ((T*)this->push(sizeof(T))) T(args...);

}

template <typename T, u64 size> T* MemoryArena::
push_array()
{

    T* result = (T*)this->push(sizeof(T) * size);
    for (u64 i = 0; i < size; ++i)
    {
        new (result + i) T();
    }

}

template <typename T, u64 size, class... Args> T* MemoryArena::
push_array(Args... args)
{

    T* result = (T*)this->push(sizeof(T) * size);
    for (u64 i = 0; i < size; ++i)
    {
        new (result + i) T(args...);
    }

}



