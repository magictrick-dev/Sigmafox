#ifndef SIGMAFOX_UTILITIES_STRING_HPP
#define SIGMAFOX_UTILITIES_STRING_HPP
#include <definitions.hpp>

// --- String ------------------------------------------------------------------
//
// Essentially, an implementation of the std::string class that uses the internal
// allocator setup rather than the standard allocator. This is to ensure that the
// string class is not dependent on the standard library, and can be used in a
// standalone manner.
//

class String
{

    public:
                        String();
                        String(u64 size, char fill = '\0');
                        String(const char* str);
                        String(const String& str);
        virtual        ~String();

        char&           operator[](int index);
        const char&     operator[](int index) const;

        String&         operator=(const String& str);
        String&         operator=(const char* str);
        String&         operator+=(const String& str);
        String&         operator+=(const char* str);
        String          operator+(const String& str) const;
        String          operator+(const char* str) const;
        bool            operator==(const String& str) const;
        bool            operator==(const char* str) const;
        bool            operator!=(const String& str) const;
        bool            operator!=(const char* str) const;

        bool            empty() const;
        void            clear();
        void            resize(u64 size, char fill = '\0');
        void            reserve(u64 size);
        u64             size() const;
        u64             length() const;

    protected:
        cptr            string_buffer;
        u64             string_size;

        friend std::ostream& operator<<(std::ostream& os, const String& str);

};

inline std::ostream& 
operator<<(std::ostream& os, const String& str)
{
    
    os << str.string_buffer;
    return os;

}

#endif
