#include <utilities/string.hpp>

String::
String()
{
    this->string_buffer = nullptr;
    this->string_size = 0;
}

String::
String(u64 size, char fill)
{
    this->string_buffer = nullptr;
    this->string_size = 0;
    this->resize(size, fill);
}

String::
String(const char* str)
{
    this->string_buffer = nullptr;
    this->string_size = 0;
    *this = str;
}

String::
String(const String& str)
{
    this->string_buffer = nullptr;
    this->string_size = 0;
    *this = str;
}

String::
~String()
{
    this->clear();
}

char& String::
operator[](int index)
{
    return this->string_buffer[index];
}

const char& String::
operator[](int index) const
{
    return this->string_buffer[index];
}

String& String::
operator=(const String& str)
{

    if (this != &str)
    {
        this->clear();
        this->resize(str.string_size);
        for (u64 i = 0; i < string_size; i++)
        {
            this->string_buffer[i] = str.string_buffer[i];
        }
    }
    return *this;

}

String& String::
operator=(const char* str)
{

    this->clear();

    if (str != nullptr)
    {
        u64 string_length = strlen(str);
        this->resize(string_length+1);
        for (u64 i = 0; i < this->string_size; i++)
        {
            this->string_buffer[i] = str[i];
        }
        this->string_buffer[string_length] = '\0';
    }

    return *this;

}

String& String::
operator+=(const String& str)
{
    this->resize(this->string_size + str.string_size);
    u64 string_length = strlen(this->string_buffer);
    for (u64 i = 0; i < str.string_size; i++)
    {
        this->string_buffer[string_length + i] = str.string_buffer[i];
    }
    return *this;
}

String& String::
operator+=(const char* str)
{
    *this += String(str);
    return *this;
}

String String::
operator+(const String& str) const
{
    String result = *this;
    result += str;
    return result;
}

String String::
operator+(const char* str) const
{
    return *this + String(str);
}

bool String::
operator==(const String& str) const
{
    if (this->string_size != str.string_size)
    {
        return false;
    }
    for (u64 i = 0; i < this->string_size; i++)
    {
        if (this->string_buffer[i] != str.string_buffer[i])
        {
            return false;
        }
    }
    return true;
}

bool String::
operator==(const char* str) const
{
    return *this == String(str);
}

bool String::
operator!=(const String& str) const
{
    return !(*this == str);
}

bool String::
operator!=(const char* str) const
{
    return !(*this == str);
}

bool String::
empty() const
{
    return this->string_size == 0;
}

void String::
clear()
{
    if (this->string_buffer != nullptr)
    {
        SF_MEMORY_FREE(this->string_buffer);
        this->string_buffer = nullptr;
    }
    this->string_size = 0;
}

void String::
resize(u64 size, char fill)
{
    if (size == 0)
    {
        this->clear();
        return;
    }
    if (this->string_size != size)
    {

        cptr new_buffer = (cptr)SF_MEMORY_ALLOC(size);
        for (i32 i = 0; i < size; i++)
        {
            new_buffer[i] = fill;
        }

        if (this->string_buffer != nullptr)
        {
            for (u64 i = 0; i < this->string_size && i < size; i++)
            {
                new_buffer[i] = this->string_buffer[i];
            }
            SF_MEMORY_FREE(this->string_buffer);
        }

        this->string_buffer = new_buffer;
        this->string_size = size;

    }
}

void String::
reserve(u64 size)
{
    if (size > this->string_size)
    {
        this->resize(size);
    }
}

u64 String::
size() const
{
    return this->string_size;
}

u64 String::
length() const
{
    return strlen(this->string_buffer);
}

