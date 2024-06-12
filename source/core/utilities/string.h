// --- String ------------------------------------------------------------------
//
// Serves as the base implementation for anything that requires string manipulations.
// Resembles std::string except it uses our custom allocator and behaves more like
// character buffer than a first class data-type.
//

#ifndef SIGMAFOX_CORE_UTILITIES_STRING_H 
#define SIGMAFOX_CORE_UTILITIES_STRING_H 
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <core/utilities/allocator.h>


class 
string 
{
    public:
        inline          string();
        inline          string(size_t init_size);
        inline          string(const char *str);
        inline          string(const string& str);
        virtual inline ~string();

        inline void     resize(size_t size);
        inline void     resize_to_fit();

        inline char*    buffer();
        inline size_t   length() const;
        inline size_t   size() const;

        inline const char * str() const;


        inline char&    operator[](size_t idx);
        inline string&  operator+=(const char *str);
        inline string&  operator+=(const string& str);

    protected:
        char   *buffer_pointer  = NULL;
        size_t  buffer_size     = NULL;

};

string::
~string()
{
    memory_free(this->buffer_pointer);
}

string::
string()
{
    this->resize(8);
}

string::
string(size_t init_size)
{
    this->resize(init_size);
}

string::
string(const char *str)
{

    // Check if we need to resize to fit.
    size_t string_length = strlen(str);
    size_t adjust_size = (((string_length + 1) / 8) + !!(string_length % 8)) * 8;
    this->resize(adjust_size);

    // Copy the string to the buffer.
    memcpy(this->buffer_pointer, str, string_length + 1);

}

string::
string(const string& str)
{
    if (this->buffer_size < str.buffer_size)
        this->resize(str.buffer_size);
    memcpy(this->buffer_pointer, str.buffer_pointer, str.buffer_size);
}

char* string::
buffer()
{
    return this->buffer_pointer;
}

size_t string::
length() const
{
    size_t length = 0;
    while (this->buffer_pointer[length] != '\0')
        length++;
    return length;
}

size_t string::
size() const
{
    return this->buffer_size;
}

void string::
resize(size_t size)
{

    // If the request size isn't big enough, return.
    if (size <= this->buffer_size) return;

    // Create the fresh buffer and then copy the old buffer over.
    char *fresh_buffer = memory_alloc_array(char, size);
    memset(fresh_buffer, 0x00, size); // Ensures null-termination.
    if (this->buffer_pointer != NULL)
        memcpy(fresh_buffer, this->buffer_pointer, this->buffer_size);

    // Release old buffer and set it to new buffer.
    if (this->buffer_pointer != NULL)
        memory_free(this->buffer_pointer);

    this->buffer_pointer = fresh_buffer;
    this->buffer_size = size;

}

void string::
resize_to_fit()
{

    size_t string_length = this->length();
    size_t adjust_size = (((string_length + 1) / 8) + !!(string_length % 8)) * 8;

    char *fresh_buffer = memory_alloc_array(char, adjust_size);
    if (this->buffer_pointer != NULL)
        memcpy(fresh_buffer, this->buffer_pointer, string_length + 1);

    if (this->buffer_pointer != NULL)
        memory_free(this->buffer_pointer);

    this->buffer_pointer = fresh_buffer;
    this->buffer_size = adjust_size;

}

char& string::
operator[](size_t idx)
{
    assert(idx < this->buffer_size);
    return this->buffer_pointer[idx];
}

string& string::
operator+=(const char *str)
{

    size_t string_length = strlen(str);
    size_t self_length = this->length();
    size_t total_length = string_length + self_length + 1;
    if (total_length >= this->buffer_size)
    {
        size_t adjust_size = ((total_length / 8) + !!(total_length % 8)) * 8;
        this->resize(adjust_size);
    }

    memcpy(this->buffer_pointer + self_length, str, string_length + 1);

    return *this;

}

string& string::
operator+=(const string& str)
{

    size_t string_length = str.length();
    size_t self_length = this->length();
    size_t total_length = string_length + self_length + 1;
    if (total_length >= this->buffer_size)
    {
        size_t adjust_size = ((total_length / 8) + !!(total_length % 8)) * 8;
        this->resize(adjust_size);
    }

    memcpy(this->buffer_pointer + self_length, str.buffer_pointer, string_length + 1);

    return *this;

}

inline const char* string::
str() const
{
    return this->buffer_pointer;
}

#endif
