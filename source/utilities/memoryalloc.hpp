#ifndef SIGMAFOX_UTILITIES_MEMORYALLOC_HPP
#define SIGMAFOX_UTILITIES_MEMORYALLOC_HPP
#include <definitions.hpp>

// --- Allocator Interface -----------------------------------------------------
//
// This interface is used to define the memory allocation strategy for the
// application. This can be changed to use a custom allocator or a default allocator
// interface as needed. The default allocator is the standard C library malloc and
// free functions. Generally, you want to use an allocator that is optimized for
// the particular use case.
//
// Updating statistics is an important task for allocators, therefore you should
// deligently implement them if you create a custom allocator.
//

struct AllocatorStatistics
{
    u64 total_allocated;
    u64 total_released;
    u64 current_allocated;
    u64 peak_allocated;
};

class IAllocator
{
    
    public:
                            IAllocator();
        virtual            ~IAllocator();

        virtual vptr        allocate(u64 size)  = 0;
        virtual void        release(vptr ptr)   = 0;

        u64                 get_total_allocated()   const;
        u64                 get_total_released()    const;
        u64                 get_current_allocated() const;
        u64                 get_peak_allocated()    const;

        static AllocatorStatistics get_global_statistics();

    protected:
        void                update_allocation_statistics(u64 size);
        void                update_release_statistics(u64 size);

    private:
        static inline AllocatorStatistics global_statistics = {0};
        AllocatorStatistics statistics = {0};

};

// --- Default Allocator -------------------------------------------------------
//
// This is the default allocator that uses the standard C library malloc and free.
//

class DefaultAllocator : public IAllocator
{
    
    public:
        virtual void*       allocate(u64 size) override;
        virtual void        release(void* ptr) override;

};

// --- Memory Arena ------------------------------------------------------------
//
// The classic stack allocator. They don't use allocate/release because it's a
// push/pop allocator. Instead, the allocation and release methods are push and
// pop. This is a simple and fast allocator that is good for temporary memory
// allocations.
//

class MemoryArena : public IAllocator
{
    
    public:
                            MemoryArena(u64 size);
        virtual            ~MemoryArena();

        virtual vptr        allocate(u64 size) override;
        virtual void        release(vptr ptr) override;

        template <typename T>                               T* push_type();
        template <typename T, class... Args>                T* push_type(Args... args);
        template <typename T, u64 size>                     T* push_array();
        template <typename T, u64 size, class... Args>      T* push_array(Args... args);

        vptr                push(u64 size);
        void                pop(u64 size);

    private:
        vptr                buffer;
        u64                 buffer_size;
        u64                 offset;

};

#endif
