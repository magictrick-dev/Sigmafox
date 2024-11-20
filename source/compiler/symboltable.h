#ifndef SOURCE_COMPILER_SYMBOLTABLE_H
#define SOURCE_COMPILER_SYMBOLTABLE_H
#include <core/definitions.h>
#include <core/arena.h>

typedef struct symbol symbol;
typedef struct symbol_table symbol_table;

typedef enum symbol_type
{
    SYMBOL_TYPE_UNDECLARED,
    SYMBOL_TYPE_UNDEFINED,
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_ARRAY,
    SYMBOL_TYPE_PROCEDURE,
    SYMBOL_TYPE_FUNCTION,
} symbol_type;

typedef struct symbol
{
    cc64 identifier;
    symbol_type type;
    u32 hash;
    b32 active;
    i32 arity;
} symbol;

typedef struct symbol_table
{
    memory_arena *arena;

    symbol_table *parent;
    symbol *symbol_buffer;

    u64 symbol_buffer_length;
    u64 symbol_buffer_count;
} symbol_table;

void            symbol_table_initialize(symbol_table *table, memory_arena *arena, u64 initialize_size);
symbol*         symbol_table_insert(symbol_table *table, cc64 identifier, symbol_type type);
symbol*         symbol_table_search_from_current_table(symbol_table *table, cc64 identifier);
symbol*         symbol_table_search_from_any_table(symbol_table *table, cc64 identifier);
r64             symbol_table_load_factor(symbol_table *table);
u32             symbol_table_hash_string(cc64 string);
b32             symbol_table_resize(symbol_table *table);
b32             symbol_table_is_adjustable(symbol_table *table);
u64             symbol_table_size(symbol_table *table);
void            symbol_table_collapse_arena(symbol_table *table);

#endif
