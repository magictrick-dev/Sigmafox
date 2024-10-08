// --- NDArray -----------------------------------------------------------------
//
//      Northern Illinois University | Written by Chris DeJong 9/2024
//
//      A dynamic type container which supports various types.
//
//      Shorthands:
//      - RE, Real
//      - CM, Complex Number
//      - LO, Logical
//      - ST, String
//
//      |----------------------------|
//      | Addition / Subtraction     |
//      |----------------------------|
//      | Left | Right | Result      |
//      |----------------------------|
//      | RE   | RE    | RE          | 
//      | RE   | CM    | CM          |
//      | CM   | RE    | CM          |
//      |----------------------------|
//
// -----------------------------------------------------------------------------

#ifndef LIBRARY_DYNAMICS_H 
#define LIBRARY_DYNAMICS_H 
#include <cassert>
#include <cstdint>
#include <iostream>

namespace Sigmafox
{

    enum class DynamicTypetag
    {
        Uninitialized,
        Numeric,
        Logical,
        String,
    };

    class Dynamic
    {
        
        public:
            inline          Dynamic();
            inline          Dynamic(int init);
            inline          Dynamic(double init);
            inline          Dynamic(bool init);
            inline          Dynamic(const char *init);
            inline virtual ~Dynamic();

            template <typename T> T& value();
            template <typename T> T value() const;

            inline size_t           size() const;
            inline DynamicTypetag   get_tag() const;

        protected:
            DynamicTypetag  tag;
            void*           storage_buffer;
            size_t          storage_size;

    };

    inline std::ostream& operator<<(std::ostream& os, const Dynamic& rhs);
    inline Dynamic operator+(const Dynamic& lhs, const Dynamic& rhs);
    inline Dynamic operator-(const Dynamic& lhs, const Dynamic& rhs);

    inline std::ostream&
    operator<<(std::ostream& os, const Dynamic& rhs)
    {

        switch (rhs.get_tag())
        {
            
            case DynamicTypetag::Uninitialized:
            {

                os << "Uninitialized";
                
            } break;

            case DynamicTypetag::Numeric:
            {

                os << rhs.value<double>();

            } break;

            case DynamicTypetag::Logical:
            {
                
                os << (rhs.value<int64_t>() != 0) ? "True" : "False";

            } break;

            case DynamicTypetag::String:
            {

                os << rhs.value<const char *>();

            } break;

        };

        return os;

    }

    inline Dynamic 
    operator+(const Dynamic& lhs, const Dynamic& rhs)
    {

        DynamicTypetag left_tag, right_tag;       
        left_tag = lhs.get_tag();
        right_tag = rhs.get_tag();

        if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Numeric)
        {

            Dynamic result(lhs.value<double>() + rhs.value<double>());
            return result;

        }

        assert(!"Operation is undefined.");
        return Dynamic(); // Because C++ will complain if I don't, though the assert
                          // will catch it before we event return.

    }

    inline Dynamic 
    operator-(const Dynamic& lhs, const Dynamic& rhs)
    {

        DynamicTypetag left_tag, right_tag;       
        left_tag = lhs.get_tag();
        right_tag = rhs.get_tag();

        if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Numeric)
        {

            Dynamic result(lhs.value<double>() - rhs.value<double>());
            return result;

        }

        assert(!"Operation is undefined.");
        return Dynamic(); // Because C++ will complain if I don't, though the assert
                          // will catch it before we event return.

    }

    template <typename T>
    T& Dynamic::
    value()
    {
        
        return *((T*)this->storage_buffer);

    }

    template <typename T>
    T Dynamic::
    value() const
    {
        
        return *((T*)this->storage_buffer);

    }

    Dynamic::
    ~Dynamic()
    {

        if (storage_buffer != nullptr)
        {
            free(storage_buffer);
            storage_buffer = nullptr;
            storage_size = 0;
        }

    }

    Dynamic::
    Dynamic()
    {

        this->tag               = DynamicTypetag::Uninitialized;
        this->storage_buffer    = nullptr;
        this->storage_size      = 0;

    }

    Dynamic::
    Dynamic(int init)
    {

        this->tag               = DynamicTypetag::Numeric;
        this->storage_buffer    = malloc(sizeof(double));
        this->storage_size      = sizeof(double);

        this->value<double>()   = init;

    }

    Dynamic::
    Dynamic(double init)
    {

        this->tag               = DynamicTypetag::Numeric;
        this->storage_buffer    = malloc(sizeof(double));
        this->storage_size      = sizeof(double);

        this->value<double>() = init;

    }

    Dynamic::
    Dynamic(bool init)
    {

        this->tag               = DynamicTypetag::Logical;
        this->storage_buffer    = malloc(sizeof(int64_t));
        this->storage_size      = sizeof(int64_t);

        this->value<int64_t>() = init;

    }

    Dynamic::
    Dynamic(const char *string)
    {

        size_t string_length = strlen(string) + 1;

        this->tag               = DynamicTypetag::String;
        this->storage_buffer    = malloc(string_length);
        this->storage_size      = string_length;

        memcpy(this->storage_buffer, string, string_length);

    }

    DynamicTypetag Dynamic::
    get_tag() const
    {
        return this->tag;
    }

}

#endif
