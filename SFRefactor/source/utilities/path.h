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
#include <iostream>
#include <definitions.h>

namespace Sigmafox 
{

    class Filepath
    {

        public:
                        Filepath();
                        Filepath(u64 initial_size);
                        Filepath(ccptr path);
                        Filepath(const Filepath& other);
            virtual    ~Filepath();

            Filepath&   operator=(const Filepath& rhs);

            Filepath&   operator+=(const Filepath& rhs);
            Filepath&   operator+=(ccptr rhs);
            bool        operator==(const Filepath& rhs) const;

            u64         size() const;
            u64         capacity() const;
            ccptr       c_str() const;

            Filepath&   canonicalize();
            bool        is_valid() const;
            bool        is_valid_directory() const;
            bool        is_valid_file() const;

        public:
            static Filepath cwd();

        protected:
            void         zero_initialize();
            void         release();
            void         resize(u64 request_size);
            u64          find_best_fit(u64 request_size) const;

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

};

#endif
