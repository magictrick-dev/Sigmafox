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

    // NOTE(Chris): Resizing is a very strict procedure that requires certain
    //              conditions to be guaranteed for it to work.

    assert(symbol_table_is_adjustable(table));
    u64 table_size = symbol_table_size(table);
    u64 resize_requirement = table_size * 2;

    // Create the working region which contains the double size, then create
    // the copy region which we move our old table to.
    void *work_region = memory_arena_push(table->arena, resize_requirement);
    void *copy_region = (u8*)work_region + table_size;

    // Copy the old table to copy region.
    memory_copy_ext(copy_region, table->symbol_buffer, table_size);

    // Clear the old table and the first half of the work regon.
    memory_set_zero_ext(table->symbol_buffer, table_size * 2);

    // Double the symbol buffer length and reset the count.
    u64 previous_length = table->symbol_buffer_length;
    table->symbol_buffer_length = previous_length * 2;
    table->symbol_buffer_count = 0;

    // From the copy buffer, iterate and rehash everything.
    symbol *symbol_array = (symbol*)copy_region;
    for (u64 i = 0; i < previous_length; ++i)
    {
        symbol *old_symbol = symbol_array + i;

        // If the symbol we just found is active, we need to move it over
        // to the new spot it needs to occupy.
        if (old_symbol->active == true)
        {

            u32 hash = old_symbol->hash;
            u32 index = (hash % table->symbol_buffer_length);
            symbol *current = table->symbol_buffer + index;
            while (true)
            {

                if (current->active == false)
                {
                    break;

                }

                else
                {

                    if (strcmp(current->identifier, old_symbol->identifier) == 0)
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

            // Move the symbol by copying it over.
            *current = *old_symbol;
            
        }
    }

    // We can now pop the copy buffer and exit.
    memory_arena_pop(table->arena, table_size);

    return true;
}

b32
symbol_table_is_adjustable(symbol_table *table)
{

    // Determine if the table is adjustable, the table would need to be located
    // at the commit_bottom - table size.
    u64 symbol_table_offset = (u64)table->symbol_buffer;
    u64 arena_offset = (u64)((u8*)table->arena->buffer + table->arena->commit_bottom);
    arena_offset -= symbol_table_size(table);

    b32 adjustable = (symbol_table_offset == arena_offset);
    return adjustable;

}

void
symbol_table_collapse_arena(symbol_table *table)
{
    
    assert(symbol_table_is_adjustable(table));
    memory_arena_pop(table->arena, symbol_table_size(table));
    return;

}

u64
symbol_table_size(symbol_table *table)
{
    u64 size = table->symbol_buffer_length * sizeof(symbol);
    return size;
}

