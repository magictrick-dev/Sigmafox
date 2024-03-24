#ifndef SIGMAFOX_CORE_DEBUGGING_H
#define SIGMAFOX_CORE_DEBUGGING_H
#include <cassert>

#if defined(SIGMAFOX_DEBUG_BUILD)
#   include <cstdio>
#   define SF_ASSERT(statement)   assert((statement))
#   define SF_NOREACH(message)    assert((!message))
#   define SF_NOIMPL(message)     assert((!message))
#   define SF_DEBUG_PRINT(format, ...) \
        { printf(format, __VA_ARGS__); }
#else
#   define SF_ASSERT(statement)
#   define SF_NOREACH(statement)
#   define SF_NOIMPL(statement)
#   define SF_DEBUG_PRINT(format, ...)
#endif

#endif
