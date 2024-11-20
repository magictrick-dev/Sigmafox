// -----------------------------------------------------------------------------
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
//      | CM   | CM    | CM          |
//      |----------------------------|
//      | Multiplication / Division  |
//      |----------------------------|
//      | Left | Right | Result      |
//      |----------------------------|
//      | RE   | RE    | RE          | 
//      | RE   | CM    | CM          |
//      | CM   | RE    | CM          |
//      | CM   | CM    | CM          |
//      |----------------------------|
//
// -----------------------------------------------------------------------------

#ifndef LIBRARY_DYNAMICS_H 
#define LIBRARY_DYNAMICS_H 
#include <vector>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <complex>
using std::complex;

namespace Sigmafox
{

    // --- Utilities -----------------------------------------------------------
    //
    // Some utility classes for the Sigmafox library. The main purpose behind
    // the allocator utility is to track whether or not the dynamic class is
    // properly matching mallocs to frees. This consistency is key since there's
    // a lot of dynamic memory allocation occuring as the dynamic type switches
    // and promotes to higher order types like complex numbers.
    //

    struct memory_stats
    {
        uint64_t bytes_allocated        = 0;
        uint64_t bytes_freed            = 0;
        uint64_t memory_allocations     = 0;
        uint64_t memory_frees           = 0;
    };

    inline std::ostream& 
    operator<<(std::ostream &os, const memory_stats& stats)
    {
        os << "Allocated: " << stats.bytes_allocated << " Freed: " << stats.bytes_freed;
        os << " F/A Ratio: " << stats.memory_frees << "/" << stats.memory_allocations;
        return os;
    }

    memory_stats *memory_get_stats() { static memory_stats _stats; return &_stats; }

    struct memory_header
    {
        uint64_t amount_allocated;
    };

    inline void* 
    memory_alloc(uint64_t size)
    {

        uint64_t size_required = sizeof(memory_header) + size;
        void *region = malloc(size_required);
        memory_header *header = (memory_header*)region;
        header->amount_allocated = size;
        void *user_offset = (uint8_t*)region + sizeof(memory_header);
        memory_stats *stats = memory_get_stats();
        stats->bytes_allocated += size;
        stats->memory_allocations += 1;
        return user_offset;

    }

    inline void
    memory_free(void *buffer)
    {

        void *region = (uint8_t*)buffer - sizeof(memory_header);
        memory_header *header = (memory_header*)region;
        memory_stats *stats = memory_get_stats();
        stats->bytes_freed += header->amount_allocated;
        stats->memory_frees += 1;

        free(region);

    }

    // --- N-Dimensional Arrays ------------------------------------------------
    //
    // Class definition for n-dimensional arrays.
    // 
    
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

        void *region = memory_alloc(size_required * sizeof(T));
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

            memory_free(packed_array);
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

    // --- Dynamic Type --------------------------------------------------------
    //
    // Type container class used during Sigmafox code generation for the transpiler.
    // Generally, you wouldn't want to directly interface with this since it causes
    // a bunch of runtime overhead when done incorrectly; so if you intend to actually
    // develop with the Sigmafox core libraries, then you will want to use the actual
    // types rather than the dynamic type itself.
    //

    enum class DynamicTypetag
    {
        Uninitialized,
        Numeric,
        Complex,
        Logical,
        String,
    };

    class Dynamic
    {
        
        public:
            inline          Dynamic();
            inline          Dynamic(const Dynamic& copy);
            inline          Dynamic(int init);
            inline          Dynamic(double init);
            inline          Dynamic(bool init);
            inline          Dynamic(complex<double> init);
            inline          Dynamic(const char *init);
            inline virtual ~Dynamic();

            inline Dynamic& operator=(const Dynamic& rhs);
            inline Dynamic& operator+=(const Dynamic& rhs);
            inline Dynamic& operator-=(const Dynamic& rhs);
            inline Dynamic& operator*=(const Dynamic& rhs);
            inline Dynamic& operator/=(const Dynamic& rhs);

            template <typename T> T& value();
            template <typename T> T value() const;

            inline size_t           size() const;
            inline DynamicTypetag   get_tag() const;

        protected:
            inline void     set_type(DynamicTypetag tag);

        protected:
            DynamicTypetag  tag;
            void*           storage_buffer;
            size_t          storage_size;

    };

    inline std::ostream& operator<<(std::ostream& os, const Dynamic& rhs);
    inline Dynamic operator+(const Dynamic& lhs, const Dynamic& rhs);
    inline Dynamic operator-(const Dynamic& lhs, const Dynamic& rhs);
    inline Dynamic operator*(const Dynamic& lhs, const Dynamic& rhs);
    inline Dynamic operator/(const Dynamic& lhs, const Dynamic& rhs);

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

            case DynamicTypetag::Complex:
            {
                
                os << rhs.value<complex<double>>();

            }

            case DynamicTypetag::Logical:
            {
                
                os << (rhs.value<int64_t>() != 0) ? "True" : "False";

            } break;

            case DynamicTypetag::String:
            {

                os << rhs.value<const char *>();

            } break;

            default:
            {
                
                assert(!"Unimplemented OStream case.");

            } break;

        };

        return os;

    }

    inline Dynamic::
    Dynamic(const Dynamic& copy)
    {

        this->storage_size      = copy.storage_size;
        this->storage_buffer    = memory_alloc(this->storage_size);
        this->tag               = copy.get_tag();
        memcpy(this->storage_buffer, copy.storage_buffer, this->storage_size);

    }

    inline Dynamic& Dynamic::
    operator=(const Dynamic& rhs)
    {

        std::cout << "Equality: " << rhs << std::endl;

    }

    inline Dynamic& Dynamic::
    operator+=(const Dynamic& rhs)
    {

        DynamicTypetag left_tag, right_tag;       
        left_tag = this->get_tag();
        right_tag = rhs.get_tag();

        if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Numeric)
        {

            this->value<double>() += rhs.value<double>();
            return *this;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Complex)
        {

            this->value<complex<double>>() += rhs.value<complex<double>>();
            return *this;
            
        }

        else if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Complex)
        {

            complex<double> result = this->value<double>() + rhs.value<complex<double>>();
            this->set_type(DynamicTypetag::Complex);
            this->value<complex<double>>() = result;
            return *this;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Numeric)
        {

            this->value<complex<double>>() += rhs.value<double>();
            return *this;

        }

        assert(!"Operation is undefined.");
        return *this; // So the compiler doesn't freak the hell out.

    }

    inline Dynamic& Dynamic::
    operator-=(const Dynamic& rhs)
    {

        DynamicTypetag left_tag, right_tag;       
        left_tag = this->get_tag();
        right_tag = rhs.get_tag();

        if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Numeric)
        {

            this->value<double>() -= rhs.value<double>();
            return *this;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Complex)
        {

            this->value<complex<double>>() -= rhs.value<complex<double>>();
            return *this;
            
        }

        else if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Complex)
        {

            complex<double> result = this->value<double>() - rhs.value<complex<double>>();
            this->set_type(DynamicTypetag::Complex);
            this->value<complex<double>>() = result;
            return *this;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Numeric)
        {

            this->value<complex<double>>() -= rhs.value<double>();
            return *this;

        }

        assert(!"Operation is undefined.");
        return *this; // So the compiler doesn't freak the hell out.

    }

    inline Dynamic& Dynamic::
    operator*=(const Dynamic& rhs)
    {

        DynamicTypetag left_tag, right_tag;       
        left_tag = this->get_tag();
        right_tag = rhs.get_tag();

        if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Numeric)
        {

            this->value<double>() *= rhs.value<double>();
            return *this;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Complex)
        {

            this->value<complex<double>>() *= rhs.value<complex<double>>();
            return *this;
            
        }

        else if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Complex)
        {

            complex<double> result = this->value<double>() * rhs.value<complex<double>>();
            this->set_type(DynamicTypetag::Complex);
            this->value<complex<double>>() = result;
            return *this;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Numeric)
        {

            this->value<complex<double>>() *= rhs.value<double>();
            return *this;

        }

        assert(!"Operation is undefined.");
        return *this; // So the compiler doesn't freak the hell out.

    }

    inline Dynamic& Dynamic::
    operator/=(const Dynamic& rhs)
    {

        DynamicTypetag left_tag, right_tag;       
        left_tag = this->get_tag();
        right_tag = rhs.get_tag();

        if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Numeric)
        {

            this->value<double>() /= rhs.value<double>();
            return *this;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Complex)
        {

            this->value<complex<double>>() /= rhs.value<complex<double>>();
            return *this;
            
        }

        else if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Complex)
        {

            complex<double> result = this->value<double>() / rhs.value<complex<double>>();
            this->set_type(DynamicTypetag::Complex);
            this->value<complex<double>>() = result;
            return *this;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Numeric)
        {

            this->value<complex<double>>() /= rhs.value<double>();
            return *this;

        }

        assert(!"Operation is undefined.");
        return *this; // So the compiler doesn't freak the hell out.

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

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Complex)
        {

            Dynamic result(lhs.value<complex<double>>() + rhs.value<complex<double>>());
            return result;
            
        }

        else if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Complex)
        {

            Dynamic result(lhs.value<double>() + rhs.value<complex<double>>());
            return result;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Numeric)
        {

            Dynamic result(lhs.value<complex<double>>() + rhs.value<double>());
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

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Complex)
        {

            Dynamic result(lhs.value<complex<double>>() - rhs.value<complex<double>>());
            return result;
            
        }

        else if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Complex)
        {

            Dynamic result(lhs.value<double>() - rhs.value<complex<double>>());
            return result;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Numeric)
        {

            Dynamic result(lhs.value<complex<double>>() - rhs.value<double>());
            return result;

        }

        assert(!"Operation is undefined.");
        return Dynamic(); // Because C++ will complain if I don't, though the assert
                          // will catch it before we event return.

    }

    inline Dynamic 
    operator*(const Dynamic& lhs, const Dynamic& rhs)
    {

        DynamicTypetag left_tag, right_tag;       
        left_tag = lhs.get_tag();
        right_tag = rhs.get_tag();

        if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Numeric)
        {

            Dynamic result(lhs.value<double>() * rhs.value<double>());
            return result;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Complex)
        {

            Dynamic result(lhs.value<complex<double>>() * rhs.value<complex<double>>());
            return result;
            
        }

        else if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Complex)
        {

            Dynamic result(lhs.value<double>() * rhs.value<complex<double>>());
            return result;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Numeric)
        {

            Dynamic result(lhs.value<complex<double>>() * rhs.value<double>());
            return result;

        }

        assert(!"Operation is undefined.");
        return Dynamic(); // Because C++ will complain if I don't, though the assert
                          // will catch it before we event return.

    }

    inline Dynamic 
    operator/(const Dynamic& lhs, const Dynamic& rhs)
    {

        DynamicTypetag left_tag, right_tag;       
        left_tag = lhs.get_tag();
        right_tag = rhs.get_tag();

        if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Numeric)
        {

            Dynamic result(lhs.value<double>() / rhs.value<double>());
            return result;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Complex)
        {

            Dynamic result(lhs.value<complex<double>>() / rhs.value<complex<double>>());
            return result;
            
        }

        else if (left_tag == DynamicTypetag::Numeric && right_tag == DynamicTypetag::Complex)
        {

            Dynamic result(lhs.value<double>() / rhs.value<complex<double>>());
            return result;

        }

        else if (left_tag == DynamicTypetag::Complex && right_tag == DynamicTypetag::Numeric)
        {

            Dynamic result(lhs.value<complex<double>>() / rhs.value<double>());
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
            memory_free(storage_buffer);
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

    inline void Dynamic::
    set_type(DynamicTypetag tag)
    {

        this->tag = tag;
        if (this->storage_buffer != nullptr) memory_free(this->storage_buffer);
        switch (tag)
        {

            case DynamicTypetag::Numeric:
            {
                this->storage_buffer = new (memory_alloc(sizeof(double))) double(0);
                this->storage_size = sizeof(double);
            } break;

            case DynamicTypetag::Complex:
            {
                this->storage_buffer = new (memory_alloc(sizeof(complex<double>))) complex<double>(0);
                this->storage_size = sizeof(complex<double>);
            } break;

        }

    }

    Dynamic::
    Dynamic(int init)
    {

        this->tag               = DynamicTypetag::Numeric;
        this->storage_buffer    = new (memory_alloc(sizeof(double))) double(init);
        this->storage_size      = sizeof(double);

    }

    Dynamic::
    Dynamic(double init)
    {

        this->tag               = DynamicTypetag::Numeric;
        this->storage_buffer    = new (memory_alloc(sizeof(double))) double(init);
        this->storage_size      = sizeof(double);

    }

    Dynamic::
    Dynamic(bool init)
    {

        this->tag               = DynamicTypetag::Logical;
        this->storage_buffer    = new (memory_alloc(sizeof(int64_t))) int64_t(init);
        this->storage_size      = sizeof(int64_t);

    }

    Dynamic::
    Dynamic(complex<double> init)
    {

        this->tag               = DynamicTypetag::Complex;
        this->storage_buffer    = new (memory_alloc(sizeof(complex<double>))) complex<double>(init);
        this->storage_size      = sizeof(complex<double>);

    }

    Dynamic::
    Dynamic(const char *string)
    {

        size_t string_length = strlen(string) + 1;

        this->tag               = DynamicTypetag::String;
        this->storage_buffer    = memory_alloc(string_length);
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
