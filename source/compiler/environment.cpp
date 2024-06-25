#include <compiler/environment.h>

void        
environment_scope_push(environment *env)
{

    symbol_table *table = memory_alloc_type(symbol_table);
    table->parent_table = env->active_table;
    table->depth        = env->active_table->depth + 1;

    env->active_table = table;

}

void        
environment_scope_pop(environment *env)
{

    symbol_table *active_table = env->active_table;

    // NOTE(Chris): If the current active table's parent table is NULL, then
    //              the currently active table is the global table. We can not
    //              pop this table, it is a catastrophic error, so assert here.
    assert(active_table->parent_table != NULL);
    env->active_table = active_table->parent_table;
    memory_free(active_table);

}

symbol*     
environment_symbol_add(environment *env, void* identifier)
{



}

symbol*     
environment_symbol_search(environment *env, void* identifier)
{

}

bool        
environment_symbol_active(environment *env, void* identifier)
{

}

