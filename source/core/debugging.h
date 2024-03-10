#ifndef POSY_CORE_DEBUGGING_H
#define POSY_CORE_DEBUGGING_H
#include <cassert>

#if defined(POSY_DEBUG_BUILD)
#   include <cstdio>
#   define POSY_ASSERT(statement)   assert((statement))
#   define POSY_NOREACH(message)    assert((!message))
#   define POSY_NOIMPL(message)     assert((!message))
#   define POSY_DEBUG_PRINT(format, ...) \
        { printf(format, __VA_ARGS__); }
#else
#   define POSY_ASSERT(statement)
#   define POSY_NOREACH(statement)
#   define POSY_NOIMPL(statement)
#   define POSY_DEBUG_PRINT(format, ...)
#endif

#endif
