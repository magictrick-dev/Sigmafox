#ifndef SIGMAFOX_CORE_DEFINITIONS_H
#define SIGMAFOX_CORE_DEFINITIONS_H
#include <cctype>
#include <cstdint>
#include <cassert>

#define SF_BYTES(n)     ((size_t)n)
#define SF_KILOBYTES(n) ((size_t)SF_BYTES(n)     * 1024)
#define SF_MEGABYTES(n) ((size_t)SF_KILOBYTES(n) * 1024)
#define SF_GIGABYTES(n) ((size_t)SF_MEGABYTES(n) * 1024)
#define SF_TERABYTES(n) ((size_t)SF_GIGABYTES(n) * 1024)

#endif
