#ifndef SOURCE_STATE_H
#define SOURCE_STATE_H
#include <core/definitions.h>
#include <core/arena.h>
#include <core/cli.h>

typedef struct state
{
    memory_arena        primary_arena;
    runtime_parameters  parameters;
} state;

inline state* 
get_state()
{
    static state __application_state = {0};
    return &__application_state;
}

#endif
