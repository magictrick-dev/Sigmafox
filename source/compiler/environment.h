// --- Environment -------------------------------------------------------------
//
// The environment is the "symbol table" of the AST. An environment is created
// at parse-time and any named identifiers are mapped as they're encountered.
// Scopes can be freely pushed and popped as they're encountered.
//
// - Symbols
//      - Symbols store the identifier, the depth they were declared, and its type.
//      - The identifier refers to the token that was mapped from the source file.
//        It's use is solely restricted for comparing identifiers.
//      - Depth plays an important role in determining if a variable is redeclared
//        or shadowed. Depending on how the language handles these cases, depth is
//        used to check against the current active scope.
//      - Type allows for usage deducation. In the future, we may want to consider
//        validating expressions which otherwise may be grammatically allowed, they
//        logically may not work, such as expression evaluation to a string where
//        a number or boolean is expected. Truthy/falsy expressions also require
//        type deducation for expressions.
//
// - Environment
//      - Contains the "global symbol table", which is the default symbol table
//        and the initially active symbol table.
//      - The active symbol table is a pointer to the top-most symbol table. It's
//        helpful for quickly evaluating if a symbol has been redefined. Symbol
//        finding occurs through walking at the start of the active symbol table
//        and going up each parent until a symbol is found or the root-most table
//        is NULL, meaning the symbol wasn't found.
//
// - Symbol Table
//      - The symbol table itself marks its own depth and parent table.
//
// TODO(Chris): The symbol table uses a linked-list to remove the STL dependency
//              from the project, but in the future we will need to create a true
//              hashmap to increase the performance dramatically. Currently,
//              the setup will work adequately, but will suffer terribly at scale.
//
// NOTE(Chris): The routines within this file use memory_alloc()/memory_free(),
//              which is necessary there isn't a clean way to ensure things get
//              freely popped from the stack allocator. In the future, we may want
//              to adjust this to work with the stack allocator by creating a
//              general allocator which interfaces with it. This means writing
//              a general allocator...
// 

#ifndef SIGMAFOX_COMPILER_ENVIRONMENT_H
#define SIGMAFOX_COMPILER_ENVIRONMENT_H
#include <core/definitions.h>
#include <core/utilities.h>
#include <compiler/token.h>

typedef enum symbol_type : uint32_t
{
    STYPE_UNINITIALIZED   = 0,
    STYPE_BOOLEAN         = 1,
    STYPE_REAL            = 2,
    STYPE_STRING          = 3,
    STYPE_PROCEDURE       = 4,
} symbol_type;

typedef struct symbol
{
    uint32_t type;
    uint32_t depth;
    void *identifier;
} symbol;

typedef struct symbol_table
{
    uint32_t depth;
    hash_table symbols;
    symbol_table *parent;
} symbol_table;

typedef struct environment
{
    symbol_table *global_table = NULL;
    symbol_table *current_table = NULL;
    uint32_t depth;
} environment;

void    environment_push_table(environment *env);
void    environment_pop_table(environment *env);
symbol* environment_get_symbol(environment *env, token *identifier, bool global);
symbol* environment_add_symbol(environment *env, token *identifier, bool global);

#endif
