// --- NDArray -----------------------------------------------------------------
//
//      Northern Illinois University | Written by Chris DeJong 9/2024
//
//      An n-dimensional array type.
//
// -----------------------------------------------------------------------------
//
// Required Feature List
// -    [X] Dynamically allocated packed-array.
// -    [X] Placement-new operator for OO-types.
// -    [X] Destructor enforcement for OO-types.
// -    [X] Correct pitch calculations.
// -    [ ] Memory layout is correct (MS to LS).
// -    [ ] Copy constructor.
// -    [ ] Assignment operator overload.
//
// Optional Feature List
// -    [ ] Bounds-checking.
// -    [ ] Iterators.
// -    [ ] Logical dimension nesting.
// -    [ ] Allocator interface for custom memory allocators.
//

#ifndef LIBRARY_NDARRAY_H
#define LIBRARY_NDARRAY_H
#include <iostream>
#include <cstdint>
#include <initializer_list>
#include <vector>

template <typename T>
class NDArray
{

    public:
                        NDArray(std::initializer_list<size_t> dimensions);
        virtual        ~NDArray();

        T&              operator[](std::initializer_list<size_t> indices);
        size_t          size() const;

    protected:
        size_t          compute_size() const;
        size_t          compute_position(std::initializer_list<size_t> indices);

        std::vector<size_t> dimensions;
        std::vector<size_t> pitches;
        T *packed_array = nullptr;

};

template <typename T> NDArray<T>::
NDArray(std::initializer_list<size_t> dimensions)
{

    size_t size_required = 1;
    for (const auto &d : dimensions)
    {
        size_required *= d;
        this->dimensions.push_back(d);
    }

    void *region = malloc(size_required * sizeof(T));
    this->packed_array = (T*)(region);
    for (size_t idx = 0; idx < size_required; ++idx)
    {
        T* current_element = this->packed_array + idx;
        current_element = new (current_element) T();
    }

    std::cout << "Array pitch(es) are: " << std::endl;
    for (int32_t i = 0; i < this->dimensions.size(); ++i)
    {
        size_t pitch = 1;
        for (int32_t j = i+1; j < this->dimensions.size(); ++j)
        {
            pitch *= this->dimensions[j];
        }
        std::cout << pitch << std::endl;
        this->pitches.push_back(pitch);
    }
    
}

template <typename T> NDArray<T>::
~NDArray()
{

    if (packed_array != nullptr)
    {

        size_t elements = this->compute_size();
        for (size_t idx = 0; idx < elements; ++idx)
        {
            T* current = this->packed_array + idx;
            current->~T();
        }

        free(packed_array);
    }

}

template <typename T> size_t NDArray<T>::
size() const
{

    size_t result = this->compute_size();
    return result;

}

template <typename T> size_t NDArray<T>::
compute_size() const
{

    size_t size = 1;
    for (const auto& d : this->dimensions) size *= d;
    return size;

}

template <typename T> size_t NDArray<T>::
compute_position(std::initializer_list<size_t> indices)
{

    size_t position = 0;
    size_t index = 0;
    for (const auto &i : indices)
        index += this->pitches[position++] * i;
    return index;

}

template <typename T> T& NDArray<T>::
operator[](std::initializer_list<size_t> indices)
{

    size_t index = this->compute_position(indices);
    T* location = this->packed_array + index;
    return *location;

}

#endif
