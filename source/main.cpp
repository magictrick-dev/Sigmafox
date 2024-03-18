// ---------------------------------------------------------------------
// POSY                 / Northern Illinois University
// Primary Maintainer   / Chris DeJong
//                      / Z1836870@students.niu.edu
//                      / magiktrikdev@gmail.com
// ---------------------------------------------------------------------
//
// This is the entry point for the POSY transpiler. Documentation is
// primarily inline, but designed to traceable starting from main so as
// to keep it simple.
//

#include <core.h>
#include <source.h>
#include <lexer.h>

#include <sys/stat.h>
#include <cstdio>

// --- Command Line Interactivity --------------------------------------
//
// A set of functions designed to help interact with the command line.
//

void
cli_display_header()
{
    printf("\n                 ____   ___  ______   __\n");
    printf("                |  _ \\ / _ \\/ ___\\ \\ / /\n");
    printf("                | |_) | | | \\___ \\\\ V / \n");
    printf("                |  __/| |_| |___) || |  \n");
    printf("                |_|    \\___/|____/ |_|  \n\n");
    printf("------------------------------------------------------------\n");
    printf("  POSY               / Northern Illinois University\n");
    printf("  Primary Maintainer / Chris DeJong\n");
    printf("                     / Z1836870@students.niu.edu\n");
    printf("                     / magiktrikdev@gmail.com\n");
    printf("------------------------------------------------------------\n");
    printf("  A parser for the COSY scripting language to C++.\n");
    printf("  To properly use POSY, provide a list of source files to\n");
    printf("  transpile and any additional parameters (if supported).\n");
    printf("------------------------------------------------------------\n\n");
    printf("  Useage: POSY [source files...]\n");
    printf("          ./POSY test.foxy includes/ex.foxy includes/eg.foxy\n\n");
}

bool
cli_validate_arguments(int argc, char ** argv)
{

    // Provides the user the necessary information to use the program.
    if (argc <= 1)
    {
        cli_display_header();
        return false;
    }

    // Cycles through all the arguments and determines if they're valid
    // file paths. If any of the file paths are invalid, fast-exit and
    // return false and indicate to the user that the path is invalid.
    for (size_t idx = 1; idx < argc; ++idx)
    {
        struct stat file_stats = {};
        if (stat(argv[idx], &file_stats) != 0)
        {
            cli_display_header();
            printf("  Error: The file path \"%s\" is invalid.\n\n", argv[idx]);
            return false;
        }
    }

    return true;

}

// --- Entry Point -----------------------------------------------------
//
// Initiates the parser routine.
//

int
main(int argc, char ** argv)
{

    // --- CLI Validation ----------------------------------------------
    //
    // Validates the command line arguments.
    //
    
    if (!cli_validate_arguments(argc, argv))
        return RETURN_STATUS_INIT_FAIL;

    // NOTE(Chris): This is a temporary check, we are only supporting one
    //              input file at a time.
    if (argc >= 3)
    {
        cli_display_header();
        printf("  Error: This is a temporary error; only one source file may\n");
        printf("         be submitted at time.\n\n");
        return RETURN_STATUS_INIT_FAIL;
    }

    // --- Source Loading ----------------------------------------------
    //
    // All command line arguments are valid here, so we can now process
    // them into memory.
    //

    source_file src = {};
    char *buffer = source_file::load_source(argv[1]);
    POSY_ASSERT(buffer != NULL); // This shouldn't happen... in theory.
    src.set_buffer(buffer);
        
    // --- Cleanup -----------------------------------------------------
    //
    // Any necessary cleanup is here before exit.
    //

    source_file::free_source(buffer);
    return RETURN_STATUS_SUCCESS;

}
