#include <runtime.h>
#include <core/arena.h>
#include <core/cli.h>
#include <platform/system.h>

int
main(int argc, char ** argv)
{

    // Create the application state.
    memory_arena primary_arena = {0};
    runtime_parameters parameters = {0};

    // Set parameter defaults.
    parameters.arg_current          = 1;
    parameters.arg_count            = argc;
    parameters.arguments            = argv;
    parameters.memory_limit         = SF_GIGABYTES(2);
    parameters.string_pool_limit    = SF_MEGABYTES(256);
    parameters.output_directory     = "./";
    parameters.output_name          = "main";

    // Parse the command line.
    if (!command_line_parse(&parameters))
    {
        printf("-- Check command line arguments.\n");
    }

    // If help was invoked, fast-exit.
    if (parameters.helped == true) return 0;

    // Allocate the primary memory arena.
    void *primary_memory_buffer = system_virtual_alloc(NULL, parameters.memory_limit);

    if (primary_memory_buffer == NULL)
    {
        printf("-- Unable to reserve memory for primary memory pool.\n");
        return 1;
    }

    memory_arena_initialize(&primary_arena, primary_memory_buffer, parameters.memory_limit);

    if (!environment_initialize(&parameters, &primary_arena))
    {
        printf("-- Transpiler initialized failed, unable to transpiler.\n");
        return 1;
    }

    return environment_runtime(&parameters, &primary_arena);

}
