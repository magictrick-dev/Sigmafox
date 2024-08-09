#include <compiler/symboltable.h>
#include <core/memops.h>

void            
symbol_table_initialize(symbol_table *table, memory_arena *arena, u64 length)
{

    assert(table != NULL);

    // Total size is length * symbol.
    u64 size = length * sizeof(symbol);
    void *symbol_buffer = memory_arena_push(arena, size);

    table->arena = arena;
    table->parent = NULL;
    table->symbol_buffer = symbol_buffer;
    table->symbol_buffer_length = length;
    table->symbol_buffer_count = 0;

    // Memory setting symbol buffer to zero ensures that entries are
    // marked as inactive.
    memory_set_zero_ext(symbol_buffer, size);

}

symbol*         
symbol_table_insert(symbol_table *table, cc64 identifier, symbol_type type)
{

    u32 hash_code = symbol_table_hash_string(identifier);
    u64 index = hash_code % table->symbol_buffer_length;

    symbol *current = table->symbol_buffer + index;
    while (true)
    {

        if (current->active == false)
        {

            break;

        }

        else
        {

            if (strcmp(current->identifier, identifier) == 0)
            {

                assert(!"Inserting on an identifier that already exists, most likely an error.");

            }
            else
            {

                index = (index + 1) % table->symbol_buffer_length;
                current = table->symbol_buffer + index;
                continue;

            }

        }

    }

    current->identifier = identifier;
    current->type = type;
    current->hash = hash_code;
    current->active = true;
    return current;

}

static inline symbol*
__symbol_table_search_at(symbol_table *table, u32 hash, cc64 string)
{

    u64 index = hash % table->symbol_buffer_length;
    symbol *current = table->symbol_buffer + index;
    while (true)
    {

        if (current->active == false) 
        {
            return NULL;
        }

        else
        {

            if (strcmp(current->identifier, string) == 0)
            {
                return current;
            }
            else
            {
                index = (index + 1) % table->symbol_buffer_length;
                current = table->symbol_buffer + index;
            }

        }

    }

    return NULL;
    

}

symbol*         
symbol_table_search_from_any_table(symbol_table *table, cc64 identifier)
{

    u32 hash = symbol_table_hash_string(identifier);

    symbol_table *current_table = table;
    symbol *result = NULL;
    while (current_table != NULL)
    {

        result = __symbol_table_search_at(table, hash, identifier);

        if (result != NULL)
        {

            break;

        }
        else
        {

            current_table = current_table->parent;

        }

    }

    return result;
}

symbol*         
symbol_table_search_from_current_table(symbol_table *table, cc64 identifier)
{

    u32 hash = symbol_table_hash_string(identifier);
    symbol *result = __symbol_table_search_at(table, hash, identifier);
    return result;

}

r64             
symbol_table_load_factor(symbol_table *table)
{
    r64 result = (r64)table->symbol_buffer_count / table->symbol_buffer_length;
    return result;
}

u32             
symbol_table_hash_string(cc64 string)
{

    u64 length = strlen(string);
    u32 hash = 0x811C9DC5; // offset basis

    for (u64 byte_index = 0; byte_index < length; ++byte_index)
    {

        hash ^= string[byte_index];
        hash *= 0x01000193; // prime

    }

    return hash;

}

b32             
symbol_table_resize(symbol_table *table)
{

    return true;
}



