#ifndef SOURCE_COMPILER_AST_H
#define SOURCE_COMPILER_AST_H
#include <compiler/parser.h>

bool    parse_tokens(array<token> *tokens, array<statement*> *statements, memory_arena *arena);
void    parser_ast_traversal_print(array<statement*> *statements);

#endif
