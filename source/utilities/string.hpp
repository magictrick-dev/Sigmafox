#ifndef SIGMAFOX_UTILITIES_STRING_HPP
#define SIGMAFOX_UTILITIES_STRING_HPP
#include <definitions.hpp>
#include <environment.hpp>

// --- String ------------------------------------------------------------------
//
// Essentially, an implementation of the std::string class that uses the internal
// allocator setup rather than the standard allocator. This is to ensure that the
// string class is not dependent on the standard library, and can be used in a
// standalone manner.
//

namespace sf
{

    class string
    {

        public:
            inline string();
            inline string(u64 size, char fill = '\0');
            inline string(const char* str);
            inline string(const string& str);
            inline virtual        ~string();

            inline char&           operator[](int index);
            inline const char&     operator[](int index) const;

            inline string&         operator=(const string& str);
            inline string&         operator=(const char* str);
            inline string&         operator+=(const string& str);
            inline string&         operator+=(const char* str);
            inline string          operator+(const string& str) const;
            inline string          operator+(const char* str) const;
            inline bool            operator==(const string& str) const;
            inline bool            operator==(const char* str) const;
            inline bool            operator!=(const string& str) const;
            inline bool            operator!=(const char* str) const;

            inline bool            empty() const;
            inline void            clear();
            inline void            resize(u64 size, char fill = '\0');
            inline void            reserve(u64 size);
            inline u64             size() const;
            inline u64             length() const;
            inline ccptr           c_ptr() const;

        protected:
            cptr            string_buffer;
            u64             string_size;

    };

    inline string::
    string()
    {
        this->string_buffer = nullptr;
        this->string_size = 0;
    }

    inline string::
    string(u64 size, char fill)
    {
        this->string_buffer = nullptr;
        this->string_size = 0;
        this->resize(size, fill);
    }

    inline string::
    string(const char* str)
    {
        this->string_buffer = nullptr;
        this->string_size = 0;
        *this = str;
    }

    inline string::
    string(const string& str)
    {
        this->string_buffer = nullptr;
        this->string_size = 0;
        *this = str;
    }

    inline string::
    ~string()
    {
        this->clear();
    }

    inline char& string::
    operator[](int index)
    {
        return this->string_buffer[index];
    }

    inline const char& string::
    operator[](int index) const
    {
        return this->string_buffer[index];
    }

    inline string& string::
    operator=(const string& str)
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

    inline string& string::
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

    inline string& string::
    operator+=(const string& str)
    {
        this->resize(this->string_size + str.string_size);
        u64 string_length = strlen(this->string_buffer);
        for (u64 i = 0; i < str.string_size; i++)
        {
            this->string_buffer[string_length + i] = str.string_buffer[i];
        }
        return *this;
    }

    inline string& string::
    operator+=(const char* str)
    {
        *this += string(str);
        return *this;
    }

    inline string string::
    operator+(const string& str) const
    {
        string result = *this;
        result += str;
        return result;
    }

    inline string string::
    operator+(const char* str) const
    {
        return *this + string(str);
    }

    inline bool string::
    operator==(const string& str) const
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

    inline bool string::
    operator==(const char* str) const
    {
        return *this == string(str);
    }

    inline bool string::
    operator!=(const string& str) const
    {
        return !(*this == str);
    }

    inline bool string::
    operator!=(const char* str) const
    {
        return !(*this == str);
    }

    inline ccptr string::
    c_ptr() const
    {
        return this->string_buffer;
    }

    inline bool string::
    empty() const
    {
        return this->string_size == 0;
    }

    inline void string::
    clear()
    {
        if (this->string_buffer != nullptr)
        {
            SF_MEMORY_FREE(this->string_buffer);
            this->string_buffer = nullptr;
        }
        this->string_size = 0;
    }

    inline void string::
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

    inline void string::
    reserve(u64 size)
    {
        if (size > this->string_size)
        {
            this->resize(size);
        }
    }

    inline u64 string::
    size() const
    {
        return this->string_size;
    }

    inline u64 string::
    length() const
    {
        return strlen(this->string_buffer);
    }

}

inline std::ostream& 
operator<<(std::ostream& os, const sf::string& str)
{
    
    os << str.c_ptr();
    return os;

}

#endif
