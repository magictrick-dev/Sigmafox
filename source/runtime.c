// --- Runtime Environment -----------------------------------------------------
//
// The runtime environment is where the magic happens.
//
#include <runtime.h>

#include <platform/fileio.h>
#include <platform/system.h>

#include <core/definitions.h>

#include <compiler/rparser.h>
#include <compiler/stringpool.h>
#include <compiler/symboltable.h>

// --- Environment Initialize --------------------------------------------------
//
// Fetch memory, resources, and other critical initialization procedures that
// must occur before the application begins.
//

int
environment_initialize(runtime_parameters *parameters, memory_arena *primary_arena)
{

    return true;

}

// --- Environment Runtime -----------------------------------------------------
//
// The residing location for the implementation details of the application. Anything
// within environment runtime should fail softly; edge-case failures must either
// assert during debug development or be handled gracefully.
//

i32
environment_runtime(runtime_parameters *parameters, memory_arena *primary_arena)
{


    u64 source_size = fileio_file_size(parameters->source_file_path);
    c64 source_buffer = malloc(sizeof(char) * (source_size + 1));
    fileio_file_read(parameters->source_file_path, source_buffer, source_size, source_size + 1);
    source_buffer[source_size] = '\0'; // Null-terminate.

    source_parser parser = {0};
    syntax_node *root = source_parser_create_ast(&parser, source_buffer, 
            parameters->source_file_path, primary_arena);
    if (root != NULL) parser_print_tree(root);
    printf("\n\n");

    if (root != NULL)
        printf("Transpilation was successful.\n");
    else
        printf("Transpilation failed. See errors for more information.\n");

    return true;

}

