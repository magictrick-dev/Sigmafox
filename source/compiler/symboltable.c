#include <compiler/symboltable.h>

void            
symbol_table_initialize(symbol_table *table, memory_arena *arena, u64 initialize_size)
{

}

symbol*         
symbol_table_insert(symbol_table *table, cc64 identifier, symbol_type type)
{

    return NULL;
}

symbol*         
symbol_table_insert_global(symbol_table *table, cc64 identifier, symbol_type type)
{

    return NULL;
}

symbol*         
symbol_table_search_from_any_table(symbol_table *table, cc64 identifier)
{

    return NULL;
}

symbol*         
symbol_table_search_from_current_table(symbol_table *table, cc64 identifier)
{

    return NULL;
}

b32             
symbol_table_resize(symbol_table *table)
{

    return true;
}



