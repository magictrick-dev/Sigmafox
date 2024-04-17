#ifndef SIGMAFOX_CORE_H
#define SIGMAFOX_CORE_H
#include <core/primitives.h>
#include <core/allocator.h>
#include <core/debugging.h>
#include <core/fileio.h>

// --- Return Codes ----------------------------------------------------
//
// The following the return codes indicate where the program failed.
//

#define RETURN_STATUS_SUCCESS           0
#define RETURN_STATUS_INIT_FAIL         1

#define RETURN_STATUS_LEXER_FAIL        4
#define RETURN_STATUS_PARSER_FAIL       5
#define RETURN_STATUS_TRANSPILE_FAIL    6

#endif
