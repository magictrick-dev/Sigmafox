// --- Array -------------------------------------------------------------------
//
// An implementation of a dynamically resizing array in C++. Functionality
// resembles that of std::vector type except it uses our tracked allocator.
//

#ifndef SIGMAFOX_CORE_UTILITIES_ARRAY_H
#define SIGMAFOX_CORE_UTILITIES_ARRAY_H
#include <new>
#include <cstring>
#include <core/definitions.h>
#include <core/utilities/allocator.h>

template <typename type>
class array
{

    public:
        inline          array();
        inline          array(const array<type>& arr);
        inline virtual ~array();

        inline array<type>&     operator=(const array<type>& arr);

        inline type&        operator[](size_t idx);
        inline const type&  operator[](size_t idx) const;

        inline type&    push(const type& instance);
        inline type&    push(type&& instance);
        inline void     clear();

        inline size_t   capacity() const;
        inline size_t   size() const;
        inline type*    buffer();

    protected:
        type       *buffer_pointer  = NULL;
        size_t      buffer_commit   = NULL; // How many elements actually used.
        size_t      buffer_size     = NULL; // How many elements available.

        inline void resize(size_t size);

};

template <typename type> array<type>::
array()
{
    this->buffer_size = 4;
    this->buffer_commit = 0;
    this->buffer_pointer = memory_allocate_array(type, 4);
}

template <typename type> array<type>::
array(const array<type>& arr)
{
    
    // Initialize.
    this->buffer_size = arr.buffer_size;
    this->buffer_commit = 0;
    this->buffer_pointer = memory_alloc_array(type, this->buffer_size);

    for (size_t idx = 0; idx < arr.buffer_commit; ++idx)
        this->push(arr[idx]);

}

template <typename type> array<type>::
~array()
{
    
    // Clears the array (invokes required destructors).
    this->clear();
    memory_release(this->buffer_pointer);

}

template <typename type> void array<type>::
clear()
{
    for (size_t idx = 0; idx < this->buffer_commit; ++idx)
    {
        type *current = this->buffer_pointer + idx;
        current->~type();
    }
}

template <typename type> size_t array<type>::
capacity() const
{
    return this->buffer_size;
}

template <typename type> size_t array<type>::
size() const
{
    return this->buffer_commit;
}

template <typename type> type* array<type>::
buffer()
{
    return this->buffer_pointer;
}

template <typename type> void array<type>::
resize(size_t size)
{
    
    // If the buffer size is already suitably sized, return.
    if (this->buffer_size >= size) return;

    // Allocate a new buffer and copy old buffer to this buffer up to the
    // amount that is actually committed.
    type *fresh_buffer = memory_allocate_array(type, size);
    memcpy(fresh_buffer, this->buffer_pointer, sizeof(type) * buffer_commit);

    // Free the old buffer.
    memory_release(this->buffer_pointer);
    this->buffer_pointer = fresh_buffer;
    this->buffer_size = size;

}

template <typename type> type& array<type>::
push(const type& instance)
{

    if (this->buffer_size <= (this->buffer_commit + 1))
    {

        // We double until we hit 512, then we continually add in 64 element
        // increments to prevent extreme bloating.
        size_t next_size = this->buffer_size;
        if (this->buffer_size < 512)
            next_size *= 2;
        else
            next_size += 64;

        this->resize(next_size);

    }

    type *element = new (this->buffer_pointer + this->buffer_commit) type(instance);
    this->buffer_commit++;
    return *element;

}

template <typename type> type& array<type>::
push(type&& instance)
{

    if (this->buffer_size <= (this->buffer_commit + 1))
    {

        // We double until we hit 512, then we continually add in 64 element
        // increments to prevent extreme bloating.
        size_t next_size = this->buffer_size;
        if (this->buffer_size < 512)
            next_size *= 2;
        else
            next_size += 64;

        this->resize(next_size);

    }

    type *element = new (this->buffer_pointer + this->buffer_commit) type(instance);
    this->buffer_commit++;
    return *element;
    
}

template <typename type> type& array<type>::
operator[](size_t idx)
{
    assert(idx < this->buffer_commit);
    return this->buffer_pointer[idx];
}

template <typename type> const type& array<type>::
operator[](size_t idx) const
{
    assert(idx < this->buffer_commit);
    return this->buffer_pointer[idx];
}

template <typename type> array<type>& array<type>::
operator=(const array<type>& arr)
{
    this->clear();
    for (size_t idx = 0; idx < arr.buffer_commit; ++idx)
        this->push(arr[idx]);
    return *this;
}

#endif
