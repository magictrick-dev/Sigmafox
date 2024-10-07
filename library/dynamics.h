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

        inline size_t           size() const;
        inline DynamicTypetag   get_tag() const;

    protected:
        DynamicTypetag  tag;
        void*           storage_buffer;
        size_t          storage_size;

};

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

    this->tag               = DynamicTypetag::Uninitialized;
    this->storage_buffer    = nullptr;
    this->storage_size      = 0;

}

Dynamic::
Dynamic(double init)
{

    this->tag               = DynamicTypetag::Uninitialized;
    this->storage_buffer    = nullptr;
    this->storage_size      = 0;

}

Dynamic::
Dynamic(bool init)
{

    this->tag               = DynamicTypetag::Uninitialized;
    this->storage_buffer    = nullptr;
    this->storage_size      = 0;

}

Dynamic::
Dynamic(const char *string)
{

    this->tag               = DynamicTypetag::Uninitialized;
    this->storage_buffer    = nullptr;
    this->storage_size      = 0;

}

#endif
