#ifndef SIGMAFOX_DEFINITIONS_HPP
#define SIGMAFOX_DEFINITIONS_HPP
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;
typedef int8_t      b8;
typedef int16_t     b16;
typedef int32_t     b32;
typedef int64_t     b64;
typedef float       r32;
typedef double      r64;
typedef const char* ccptr;
typedef char*       cptr;
typedef void*       vptr;

#define SF_BYTES(n)         ((u64)n)
#define SF_KILOBYTES(n)     (SF_BYTES(n)        * 1024)
#define SF_MEGABYTES(n)     (SF_KILOBYTES(n)    * 1024)
#define SF_GIGABYTES(n)     (SF_MEGABYTES(n)    * 1024)
#define SF_TERABYTES(n)     (SF_GIGABYTES(n)    * 1024)

#define SF_ASSERT(stm)      (assert(stm))
#define SF_ENSURE_PTR(ptr)  (assert((ptr != NULL)))
#define SF_NO_IMPL(msg)     (assert(!(msg)))

#define SF_MEMORY_ALLOC(size)   (malloc(size))
#define SF_MEMORY_FREE(ptr)     (free(ptr))

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::vector;
using std::unordered_map;
using std::stack;

inline void memop_string_copy(char* dest, const char* src, u64 size)
{

    SF_ASSERT(dest != nullptr);
    SF_ASSERT(src != nullptr);
    SF_ASSERT(size > 0);

    for (u64 i = 0; i < size; ++i)
    {
        dest[i] = src[i];
    }

}

#endif
