#ifndef SIGMAFOX_UTILITIES_VECTOR_HPP
#define SIGMAFOX_UTILITIES_VECTOR_HPP
#include <definitions.hpp>

template <typename T>
class Vector
{
    public:
        inline              Vector();
        inline              Vector(i32 capacity);
        inline              Vector(i32 capacity, i32 growth_factor);
        inline              Vector(const Vector<T>& other);
        inline              Vector(Vector<T>&& other);
        inline virtual     ~Vector();

        inline void         push_back(const T& value);
        inline void         push_back(T&& value);
        inline void         pop_back();
        inline void         clear();
        inline void         reserve(i32 capacity);
        inline void         resize(i32 size);
        inline void         resize(i32 size, const T& value);
        inline void         shrink_to_fit();
        inline void         swap(Vector<T>& other);

        inline T&           operator[](i32 index);
        inline const T&     operator[](i32 index) const;

        inline i32          size() const;
        inline i32          capacity() const;
        inline bool         empty() const;

        inline T*           data();
        inline const T*     data() const;

    private:
        inline void         grow();
        inline void         grow(i32 capacity);
        inline void         shrink();
        inline void         shrink(i32 capacity);

    private:
        T* buffer           = nullptr;
        i32 count           = 0;
        i32 commit          = 0;
        i32 growth_factor   = 2;
};

template <typename T> Vector<T>::
Vector()
{
    this->count = 0;
    this->commit = 0;
    this->growth_factor = 2;
    this->resize(8);
}

template <typename T> Vector<T>::
Vector(i32 capacity)
{
    this->count = 0;
    this->commit = 0;
    this->growth_factor = 2;
    this->resize(capacity);
}

template <typename T> Vector<T>::
Vector(i32 capacity, i32 growth_factor)
{
    this->count = 0;
    this->commit = capacity;
    this->growth_factor = growth_factor;
    this->resize(capacity);
}

template <typename T> Vector<T>::
Vector(const Vector<T>& other)
{

    this->buffer = new T[other.capacity];
    this->count = other.count;
    this->commit = other.capacity;
    this->growth_factor = other.growth_factor;

    for (i32 i = 0; i < this->count; i++)
    {
        this->buffer[i] = other.buffer[i];
    }
}

template <typename T> Vector<T>::
Vector(Vector<T>&& other)
{
    this->buffer = other.buffer;
    this->count = other.count;
    this->commit = other.capacity;
    this->growth_factor = other.growth_factor;

    other.buffer = nullptr;
    other.count = 0;
    other.capacity = 0;
    other.growth_factor = 2;
}

template <typename T> Vector<T>::
~Vector()
{
    if (this->buffer != nullptr)
    {
        delete[] this->buffer;
    }
}

template <typename T> void Vector<T>::
push_back(const T& value)
{
    if (this->count == this->commit)
    {
        this->grow();
    }

    this->buffer[this->count++] = value;
}

template <typename T> void Vector<T>::
push_back(T&& value)
{
    if (this->count == this->commit)
    {
        this->grow();
    }

    this->buffer[this->count++] = value;
}

template <typename T> void Vector<T>::
pop_back()
{
    if (this->count > 0)
    {
        this->count--;

        if (this->count < this->commit / 2)
        {
            this->shrink();
        }
    }
}

template <typename T> void Vector<T>::
clear()
{
    this->count = 0;
    this->shrink();
}

template <typename T> void Vector<T>::
reserve(i32 capacity)
{
    if (capacity > this->commit)
    {
        this->grow(capacity);
    }
}

template <typename T> void Vector<T>::
resize(i32 size)
{
    if (size > this->commit)
    {
        this->grow(size);
    }

}

template <typename T> void Vector<T>::
resize(i32 size, const T& value)
{
    if (size > this->commit)
    {
        this->grow(size);
    }

    for (i32 i = this->count; i < size; i++)
    {
        this->buffer[i] = value;
    }

}

template <typename T> void Vector<T>::
shrink_to_fit()
{
    this->shrink();
}

template <typename T> void Vector<T>::
swap(Vector<T>& other)
{
    T* buffer = this->buffer;
    i32 count = this->count;
    i32 capacity = this->commit;
    i32 growth_factor = this->growth_factor;

    this->buffer = other.buffer;
    this->count = other.count;
    this->commit = other.capacity;
    this->growth_factor = other.growth_factor;

    other.buffer = buffer;
    other.count = count;
    other.capacity = capacity;
    other.growth_factor = growth_factor;
}

template <typename T> T& Vector<T>::
operator[](i32 index)
{
    return this->buffer[index];
}

template <typename T> const T& Vector<T>::
operator[](i32 index) const
{
    return this->buffer[index];
}

template <typename T> i32 Vector<T>::
size() const
{
    return this->count;
}

template <typename T> i32 Vector<T>::
capacity() const
{
    return this->commit;
}

template <typename T> bool Vector<T>::
empty() const
{
    return this->count == 0;
}

template <typename T> T* Vector<T>::
data()
{
    return this->buffer;
}

template <typename T> const T* Vector<T>::
data() const
{
    return this->buffer;
}

template <typename T> void Vector<T>::
grow()
{
    i32 new_capacity = this->commit * this->growth_factor;
    this->grow(new_capacity);
}

template <typename T> void Vector<T>::
grow(i32 capacity)
{
    T* buffer = new T[capacity];

    for (i32 i = 0; i < this->count; i++)
    {
        buffer[i] = this->buffer[i];
    }

    if (this->buffer != nullptr)
    {
        delete[] this->buffer;
    }

    this->buffer = buffer;
    this->commit = capacity;
}

template <typename T> void Vector<T>::
shrink()
{
    i32 new_capacity = this->commit / 2;
    T* buffer = new T[new_capacity];

    for (i32 i = 0; i < this->count; i++)
    {
        buffer[i] = this->buffer[i];
    }

    if (this->buffer != nullptr)
    {
        delete[] this->buffer;
    }

    this->buffer = buffer;
    this->commit = new_capacity;
}

template <typename T> void Vector<T>::
shrink(i32 capacity)
{
    T* buffer = new T[capacity];

    for (i32 i = 0; i < this->count; i++)
    {
        buffer[i] = this->buffer[i];
    }

    if (this->buffer != nullptr)
    {
        delete[] this->buffer;
    }

    this->buffer = buffer;
    this->commit = capacity;
}

#endif
