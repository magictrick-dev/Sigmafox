#include <core/arena.h>
#include <core/cli.h>
#include <platform/system.h>
#include <compiler/rparser.h>
#include <compiler/generation.h>

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

    // Initialize the primary memory arena.
    memory_arena_initialize(&primary_arena, primary_memory_buffer, parameters.memory_limit);

    // Begin the parser routine.
    source_parser parser = {0};
    syntax_node *root = source_parser_create_ast(&parser, parameters.source_file_path, &primary_arena);

    if (root != NULL)
    {
        //parser_print_tree(root);
        transpile_syntax_tree(root, &primary_arena, "project.cpp");
        
        printf("-- Transpilation was successful.\n");
        return 0;
    }

    else
    {
        printf("-- Transpilation failed.\n");
        return 1;
    }

}
