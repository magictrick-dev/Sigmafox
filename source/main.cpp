#include <iostream>
#include <iomanip>
#include <string>
#include <platform/filesystem.hpp>
#include <platform/system.hpp>
#include <utilities/path.hpp>
#include <utilities/cli.hpp>
#include <compiler/compiler.hpp>
#include <compiler/generation/sourcefile.hpp>

int
main(int argc, char ** argv)
{


    {

        // --- Command Line Parsing ------------------------------------------------
        //
        //
        // Validates and establishes the environment context based on CLI parameters.
        //

        Filepath user_source_file = Filepath::cwd();
        if (!CLI::parse(argc, argv) || argc <= 1)
        {
            CLI::short_help();
            return -1;
        }
        else
        {

            // Check if the first argument is indeed a string argument.
            CLIArgument *file_argument = CLI::get(1);
            if (file_argument->get_type() != CLIArgumentType::String)
            {
                CLI::short_help();
                std::cout << std::endl;
                std::cout << "CLI Error: Expected a string argument at argument position 1." << std::endl;
                return -1;
            }

            // Construct the path and canonicalize it.
            user_source_file += "/";
            user_source_file += file_argument->get_argument();
            user_source_file.canonicalize();

            // Check if the file exists.
            if (!user_source_file.is_valid_file())
            {
                CLI::short_help();
                std::cout << std::endl;
                std::cout << "CLI Error: Expected a valid path to a file at argument position 1." << std::endl;
                return -1;
            }


        } 

#if 0
        GeneratableSourcefile test_source("test", "test.cpp");
        test_source
            .insert_line("Hello world.")
            .insert_line("Foo.")
            .push_tabs()
            .insert_line_with_tabs("Bar.")
            .pop_tabs()
            .insert_line("Hello ")
            .append_to_current_line("world.");
        std::cout << test_source.get_source() << std::endl;
#endif

// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
#define SF_REPITITION_TEST 0
#define SF_REPITITION_COUNT (4*1024)
#if SF_REPITITION_TEST == 1
        for (int i = 0; i < SF_REPITITION_COUNT; ++i)
        {
#endif
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000

        Compiler compiler(user_source_file.c_str());
        if (!compiler.parse())
        {
            std::cout << "The compiler wasn't able to parse the source file." << std::endl;
            return -1;
        }
        else
        {
            if (!compiler.validate())
            {
                std::cout << "The compiler wasn't able to validate the AST." << std::endl;
                return -1;
            }
            else
            {
                if (!compiler.generate())
                {
                    std::cout << "The compiler wasn't able to generate the output files." << std::endl;
                    return -1;
                }
            }
        }

// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
#if SF_REPITITION_TEST == 1
        }
#endif
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000
// CPU BURNER 9000

    }


    return 0;

}
