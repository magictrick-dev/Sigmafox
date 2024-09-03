#ifndef SIGMAFOX_CORE_DEFINITIONS_H
#define SIGMAFOX_CORE_DEFINITIONS_H
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#define SF_BYTES(n)     ((size_t)n)
#define SF_KILOBYTES(n) ((size_t)SF_BYTES(n)     * 1024)
#define SF_MEGABYTES(n) ((size_t)SF_KILOBYTES(n) * 1024)
#define SF_GIGABYTES(n) ((size_t)SF_MEGABYTES(n) * 1024)
#define SF_TERABYTES(n) ((size_t)SF_GIGABYTES(n) * 1024)

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
typedef char*       c64;
typedef const char* cc64;

#if !defined(__cplusplus)
#define false   0
#define true    1
#endif

#if defined(__cplusplus)
#define externable extern "C"
#else
#define externable
#endif

#endif
