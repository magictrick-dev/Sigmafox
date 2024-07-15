#include <compiler/environment.h>

void    
environment_push_table(environment *env)
{

    symbol_table *current_table = env->current_table;
    symbol_table *new_table = memory_allocate_type(symbol_table);
    hash_table_create(&new_table->symbols, sizeof(symbol), 32, .75f, hash_function_fnv1a);
    new_table->parent = current_table;
    new_table->depth = env->depth++;
    env->current_table = new_table;

}

void    
environment_pop_table(environment *env)
{

    symbol_table *current_table = env->current_table;
    assert(current_table != NULL);

    symbol_table *parent_table = current_table->parent;

    hash_table_release(&current_table->symbols);
    memory_release(current_table);

    env->current_table = parent_table;
    env->depth--;

}

symbol* 
environment_get_symbol(environment *env, token *identifier, bool global)
{

    uint64_t token_length = identifier->length + 1;
    char *token_buffer = memory_allocate_array(char, token_length);
    token_copy_string(identifier, token_buffer, token_length, 0);

    symbol *result = NULL; 
    symbol_table *current_table = env->current_table;
    if (global == true) current_table = env->global_table;
    while (current_table != NULL)
    {

        symbol *find_result = hash_table_find_type(&current_table->symbols, token_buffer, symbol);
        if (find_result != NULL)
            return find_result;

        current_table = current_table->parent;
    }

    memory_release(token_buffer);
    return NULL; // Wasn't found.

}

symbol* 
environment_add_symbol(environment *env, token *identifier, bool global)
{

    uint64_t token_length = identifier->length + 1;
    char *token_buffer = memory_allocate_array(char, token_length);
    token_copy_string(identifier, token_buffer, token_length, 0);

    symbol_table *current_table = env->current_table;
    if (global == true) current_table = env->global_table;
    assert(current_table != NULL);

    symbol *new_symbol = hash_table_insert_type(&current_table->symbols, token_buffer, symbol);

    memory_release(token_buffer);
    return new_symbol;

}


