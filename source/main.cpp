#include <iostream>
#include <iomanip>
#include <string>
#include <platform/filesystem.hpp>
#include <platform/system.hpp>
#include <utilities/path.hpp>
#include <utilities/cli.hpp>
#include <compiler/compiler.hpp>

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
    }


    return 0;

}
