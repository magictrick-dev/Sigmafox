#ifndef POSY_CORE_PRIMITIVES_H
#define POSY_CORE_PRIMITIVES_H
#include <cstdint>
#include <cstddef>

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;

typedef int8_t          i8;
typedef int16_t         i16;
typedef int32_t         i32;
typedef int64_t         i64;

typedef float           r32;
typedef double          r64;

typedef int8_t          b8;
typedef int16_t         b16;
typedef int32_t         b32;
typedef int64_t         b64;

typedef uint64_t        s64;

#define POSYPERSIST     static
#define POSYINTERNAL    static
#define POSYINLINE      inline

#endif
