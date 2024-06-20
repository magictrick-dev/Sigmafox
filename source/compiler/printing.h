// --- Printing ----------------------------------------------------------------
//
// A set of printing functions for user-output regarding each step in the compiler
// such as error printing and debug output.
//

#ifndef SIGMAFOX_COMPILER_PRINTING_H
#define SIGMAFOX_COMPILER_PRINTING_H
#include <core/definitions.h>
#include <compiler/parser.h>

inline void
print_symbol_error(string where, token *what)
{
    string token_string = parser_token_to_string(what);
    printf("%s(%lld, %lld): Undefined symbol '%s' encountered.\n",
            where.str(),
            what->line,
            what->offset_from_start - what->offset_from_line,
            token_string.str());
    return;
}

inline void
print_symbol(string where, token *what)
{
    string token_string = parser_token_to_string(what);
    string type_string = parser_token_type_to_string(what);
    printf("%s(%lld, %lld): Token %s encountered: '%s'\n",
            where.str(),
            what->line,
            what->offset_from_start - what->offset_from_line,
            type_string.str(),
            token_string.str());
    return;
}

#endif
