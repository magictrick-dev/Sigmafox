//
//
//

#include <core.h>

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
    printf("     eg.: ./POSY test.foxy includes/ex.foxy includes/eg.foxy\n\n");
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

    // Validates the command line arguments.
    if (cli_validate_arguments(argc, argv)) return 1;

    // All command line arguments are valid here, so we can now process
    // them into memory.

    return 0;

}
