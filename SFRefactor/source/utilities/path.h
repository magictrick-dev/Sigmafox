// --- Sigmafox Filepath Utility -----------------------------------------------
//
// Written by Chris DeJong Oct. 2024 / Northern Illinois University
//
//      The filepath utility provides simplified interface for constructing and
//      using filepaths. It behaves pretty similarily to a string class in that
//      it dynamically allocates storage to hold a buffer of characters. The
//      main advantage of this interface is that ability to quickly construct,
//      canonicalize, and verify paths without using the lower-level platform API.
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_UTILITIES_PATH_H
#define SIGMAFOX_UTILITIES_PATH_H
#include <definitions.h>

namespace Sigmafox 
{

    class Filepath
    {

        public:
            inline              Filepath();
            inline              Filepath(u64 initial_size);
            inline              Filepath(ccptr path);
            inline              Filepath(const Filepath& other);
            inline virtual     ~Filepath();

            inline Filepath&    operator=(const Filepath& rhs);

/*
            inline Filepath&    operator+=(const Filepath& rhs);
            inline Filepath&    operator+=(ccptr rhs);
*/

            inline u64          size() const;
            inline u64          capacity() const;
            inline ccptr        c_str() const;

            inline Filepath&    canonicalize();
            inline bool         is_valid() const;
            inline bool         is_valid_directory() const;
            inline bool         is_valid_file() const;
/*
            inline Filepath&    pop_filename();
            inline Filepath&    pop_directory();

*/

        public:
            static inline Filepath cwd();

        protected:
            inline void         zero_initialize();
            inline void         release();
            inline u64          find_best_fit(u64 request_size) const;

        protected:
            cptr    buffer_ptr;
            u32     buffer_length;
            u32     buffer_capacity;

        friend inline std::ostream& operator<<(std::ostream& lhs, const Filepath& rhs);
    };

    inline std::ostream& 
    operator<<(std::ostream& lhs, const Filepath& rhs)
    {
        if (rhs.buffer_ptr != nullptr)
        {
            lhs << rhs.buffer_ptr;
        }
        return lhs;
    }

    inline Filepath::
    Filepath()
    {
        this->zero_initialize();
    }

    inline Filepath::
    Filepath(u64 initial_size)
    {

        this->zero_initialize();
        if (initial_size > 0)
        {
            u64 required_size = this->find_best_fit(initial_size);
            this->buffer_ptr        = (char*)SF_MEMORY_ALLOC(required_size);
            this->buffer_capacity   = required_size;
            this->buffer_length     = 0;
            this->buffer_ptr[0]     = '\0';
        }

    }

    inline Filepath::
    Filepath(ccptr path)
    {

        this->zero_initialize();

        u64 string_length = strlen(path);
        u64 required_size = this->find_best_fit(string_length + 1);
        this->buffer_ptr        = (char*)SF_MEMORY_ALLOC(required_size);
        this->buffer_capacity   = required_size;
        this->buffer_length     = string_length;
        strcpy(this->buffer_ptr, path);

    }

    inline Filepath::
    Filepath(const Filepath& other)
    {

        this->zero_initialize();
        this->buffer_ptr = (char*)SF_MEMORY_ALLOC(other.buffer_capacity);
        this->buffer_length = other.buffer_length;
        this->buffer_capacity = other.buffer_capacity;
        strcpy(this->buffer_ptr, other.buffer_ptr);

    }

    inline Filepath& Filepath::
    operator=(const Filepath& rhs)
    {
        
        this->release();
        this->zero_initialize();
        this->buffer_ptr = (char*)SF_MEMORY_ALLOC(rhs.buffer_capacity);
        this->buffer_length = rhs.buffer_length;
        this->buffer_capacity = rhs.buffer_capacity;
        strcpy(this->buffer_ptr, rhs.buffer_ptr);

    }

    inline Filepath::
    ~Filepath()
    {
        this->release();
    }

    inline void Filepath::
    zero_initialize()
    {
        this->buffer_ptr = nullptr;
        this->buffer_length = 0;
        this->buffer_capacity = 0;
    }

    inline void Filepath::
    release()
    {
        if (this->buffer_ptr != nullptr)
            SF_MEMORY_FREE(this->buffer_ptr);
    }

    inline u64 Filepath::
    find_best_fit(u64 request_size) const
    {

        u64 result = ((request_size / 32) + (request_size % 32 != 0)) * 32;
        return result;

    }

    inline u64 Filepath::
    size() const
    {
        return this->buffer_length;
    }

    inline u64 Filepath::
    capacity() const
    {
        return this->buffer_capacity;
    }

    inline ccptr Filepath::
    c_str() const
    {
        return this->buffer_ptr;
    }

    inline Filepath& Filepath::
    canonicalize()
    {

        char buffer[260];
        file_canonicalize_path(260, buffer, this->c_str());
            
        this->release();
        this->zero_initialize();
        u64 string_length = strlen(buffer);
        u64 required_size = this->find_best_fit(string_length + 1);
        this->buffer_ptr        = (char*)SF_MEMORY_ALLOC(required_size);
        this->buffer_capacity   = required_size;
        this->buffer_length     = string_length;
        strcpy(this->buffer_ptr, buffer);

        return *this;

    }

    inline bool Filepath::
    is_valid_directory() const
    {

        bool result = (file_is_directory(this->c_str()));
        return result;

    }

    inline bool Filepath::
    is_valid_file() const
    {

        bool result = (file_is_file(this->c_str()));
        return result;

    }

    inline bool Filepath::
    is_valid() const
    {

        bool result = (this->is_valid_directory() || this->is_valid_file());
        return result;

    }

    inline Filepath Filepath::
    cwd()
    {
        
        char buffer[260];
        u32 size = file_current_working_directory(260, buffer);
        Filepath path(buffer);
        return path;

    }


};

#endif
