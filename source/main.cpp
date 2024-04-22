// -----------------------------------------------------------------------------
//  ____ ___ ____ __  __    _    _____ _____  __
// / ___|_ _/ ___|  \/  |  / \  |  ___/ _ \ \/ /
// \___ \| | |  _| |\/| | / _ \ | |_ | | | \  / 
//  ___) | | |_| | |  | |/ ___ \|  _|| |_| /  \
// |____/___\____|_|  |_/_/   \_\_|   \___/_/\_\
//                                             
// -----------------------------------------------------------------------------
// Sigmafox             / Northern Illinois University
// Primary Maintainer   / Chris DeJong
//                      / Z1836870@students.niu.edu
//                      / magiktrikdev@gmail.com
// -----------------------------------------------------------------------------

#include <core.h>
#include <commandline.h>
#include <source.h>
#include <lexer.h>
#include <generate.h>

#include <iostream>
#include <cstdio>

// --- Colored Output Testing --------------------------------------------------

void
test_cli_output()
{

    std::cout << " \t\t8-bit Color Table" << std::endl;
    for (int i = 0; i < 108; ++i)
    {
        if (i % 9 == 0 && i != 0)
            std::cout << std::endl;

        printf("\033[%dm %3d\033[m", i, i);
    }
    std::cout << std::endl;

    std::cout << " \t\t16-bit Color Table" << std::endl;
    for (int i = 0; i < 256; i++)
    {
        if (i % 16 == 0 && i != 0)
            std::cout << std::endl;
        printf("\033[38;5;%dm %3d hello\033[m", i, i);
    }
    std::cout << std::endl;

}

// --- Input Runtime Functions -------------------------------------------------

bool
cli_validate_arguments(int argc, char ** argv)
{

    // Provides the user the necessary information to use the program.
    if (argc <= 1)
    {
        sigmafox_cli_print_header();
        sigmafox_cli_print_description();
        sigmafox_cli_print_useage();
        return false;
    }

    // Cycles through all the arguments and determines if they're valid
    // file paths. If any of the file paths are invalid, fast-exit and
    // return false and indicate to the user that the path is invalid.
    for (size_t idx = 1; idx < argc; ++idx)
    {
        if (!sigmafox_file_exists(argv[idx]))
        {
            sigmafox_cli_print_header();
            sigmafox_cli_print_description();
            sigmafox_cli_print_useage();
            printf("  Error: The file %s was not found.\n\n", argv[idx]);
            return false;
        }
    }

    return true;

}

// --- Entry Point -------------------------------------------------------------
//
// Initiates the parser routine.
//

int
main(int argc, char ** argv)
{

    // --- CLI Validation ------------------------------------------------------
    //
    // Validates the command line arguments.
    //
    
    if (!cli_validate_arguments(argc, argv)) return RETURN_STATUS_INIT_FAIL;
    sigmafox_cli_print_header();

    // NOTE(Chris): This is a temporary check, we are only supporting one
    //              input file at a time.
    if (argc >= 3)
    {
        sigmafox_cli_print_useage();
        printf("  Error: This is a temporary error; only one source file may\n");
        printf("         be submitted at time.\n\n");
        return RETURN_STATUS_INIT_FAIL;
    }

    // Testing CLI output.
    test_cli_output();

    // --- Source File Loading -------------------------------------------------
    // 
    // Loads all the source files the user provided into memory and then does a
    // pre-parse on them to break them up into line-by-line segments.
    //

#if 0
    // Loads the source into memory.
    size_t source_size = sigmafox_file_size(argv[1]);
    char *source_buffer = (char*)sigmafox_memory_alloc(source_size + 1);
    sigmafox_file_read_text(argv[1], source_buffer, source_size, source_size + 1);

    Lex source_lex(source_buffer, argv[1]);
    if (source_lex.has_errors())
    {
        source_lex.print_errors();
        return RETURN_STATUS_LEXER_FAIL;
    }
    else
    {
        source_lex.print_tokens();
    }
#endif

    Typename square_signature = { "ShapeSquare", "ShapeSquare" };
    ClassDefinition square_class(square_signature, NULL);



    // --- Cleanup -------------------------------------------------------------
    //
    // Any necessary cleanup is here before exit.
    //

    return RETURN_STATUS_SUCCESS;

}
