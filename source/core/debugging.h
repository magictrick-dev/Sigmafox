#ifndef POSY_CORE_DEBUGGING_H
#define POSY_CORE_DEBUGGING_H
#include <cassert>

#if defined(POSY_DEBUG_BUILD)
#   define POSY_ASSERT(statement)   assert((statement))
#   define POSY_NOREACH(message)    assert((!message))
#   define POSY_NOIMPL(message)     assert((!message))
#else
#   define POSY_ASSERT(statement)
#   define POSY_NOREACH(statement)
#   define POSY_NOIMPL(statement)
#endif

#endif
