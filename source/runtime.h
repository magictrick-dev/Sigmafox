#ifndef SIGMAFOX_RUNTIME_H
#define SIGMAFOX_RUNTIME_H
#include <core/definitions.h>
#include <core/arena.h>
#include <core/cli.h>

i32 environment_initialize(runtime_parameters *parameters, memory_arena *primary_arena);
i32 environment_runtime(runtime_parameters *parameters, memory_arena *primary_arena);

#endif
