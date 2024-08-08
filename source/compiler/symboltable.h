#ifndef SOURCE_COMPILER_SYMBOLTABLE_H
#define SOURCE_COMPILER_SYMBOLTABLE_H
#include <core/definitions.h>
#include <core/arena.h>

typedef struct symbol symbol;
typedef struct symbol_table symbol_table;

typedef enum symbol_type
{
    SYMBOL_TYPE_UNDEFINED,
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_PROCEDURE,
    SYMBOL_TYPE_FUNCTION,
} symbol_type;

typedef struct symbol
{
    cc64 identifier;
    symbol_type type;
} symbol;

typedef struct symbol_table
{
    memory_arena *arena;

    symbol_table *parent;
    symbol *symbol_buffer;

    u64 symbol_buffer_length;
    u64 symbol_buffer_count;
    r64 load_factor;
} symbol_table;

void            symbol_table_initialize(symbol_table *table, memory_arena *arena, u64 initialize_size);
symbol*         symbol_table_insert(symbol_table *table, cc64 identifier, symbol_type type);
symbol*         symbol_table_insert_global(symbol_table *table, cc64 identifier, symbol_type type);
symbol*         symbol_table_search_from_current_table(symbol_table *table, cc64 identifier);
symbol*         symbol_table_search_from_any_table(symbol_table *table, cc64 identifier);
b32             symbol_table_resize(symbol_table *table);

#endif
