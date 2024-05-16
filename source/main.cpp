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
    sigmafox_cli_print_header();

#if 0
    // --- CLI Validation ------------------------------------------------------
    //
    // Validates the command line arguments.
    //
    
    if (!cli_validate_arguments(argc, argv)) return RETURN_STATUS_INIT_FAIL;

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

    Typename square_signature = { "ShapeSquare", "ShapeSquare" };
    ClassDefinition square_class(square_signature, NULL);

    Methodname square_area;
    square_area.signature = { "int", "get_area" };
    square_class.add_method(square_area, EScope::Public);
    
    Methodname square_perimeter;
    square_perimeter.signature = { "int", "get_perimter" };
    square_class.add_method(square_perimeter, EScope::Public);

    Methodname square_set_width;
    square_set_width.signature = { "void", "set_width" };
    square_set_width.parameters.push_back({"int", "width"});
    square_class.add_method(square_set_width, EScope::Public);

    Methodname square_set_height;
    square_set_height.signature = { "void", "set_height" };
    square_set_height.parameters.push_back({"int", "height"});
    square_class.add_method(square_set_height, EScope::Public);

    Typename square_width   = { "int", "width" };
    Typename square_height  = { "int", "height" };
    square_class.add_property(square_width, EScope::Private);
    square_class.add_property(square_height, EScope::Private);

    std::string output_class = square_class.generate(4);
    std::cout << "\n\n" << output_class << std::endl;
#endif

    SourceFunctionDecleration add_numbers({"int", "add_numbers"});
    add_numbers.append_parameter({"int", "left_operand"});
    add_numbers.append_parameter({"int", "right_operand"});

    SourceFunctionDecleration multiply_numbers({"int", "mul_numbers"});
    multiply_numbers.append_parameter({"int", "left_operand"});
    multiply_numbers.append_parameter({"int", "right_operand"});

    SourceHeaderDocument shape_document;
    shape_document.append_function(add_numbers);
    shape_document.append_function(multiply_numbers);

    std::string shape_source = shape_document.to_string(0);
    std::cout << shape_source << std::endl;

    // --- Cleanup -------------------------------------------------------------
    //
    // Any necessary cleanup is here before exit.
    //

    return RETURN_STATUS_SUCCESS;

}
