// --- NDArray -----------------------------------------------------------------
//
//      Northern Illinois University | Written by Chris DeJong 9/2024
//
//      A dynamic type container.
//
// -----------------------------------------------------------------------------
#ifndef LIBRARY_DYNAMICS_H 
#define LIBRARY_DYNAMICS_H 
#include <cstdint>

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

        inline size_t           size() const;
        inline DynamicTypetag   get_tag() const;

    protected:
        DynamicTypetag  tag;
        void*           storage_buffer;
        size_t          storage_size;

};

template <typename T>
T& Dynamic::
value()
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

    this->value<double>() = init;

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

#endif
