#include <iostream>
#include <iomanip>
#include <string>
#include <environment.hpp>
#include <platform/filesystem.hpp>
#include <platform/system.hpp>
#include <utilities/path.hpp>
#include <utilities/cli.hpp>
#include <utilities/vector.hpp>
#include <utilities/string.hpp>

#if 0
#include <compiler/tokenizer.hpp>
#include <compiler/parser.hpp>
#include <compiler/syntaxtree.hpp>
#include <compiler/visitors/reference.hpp>
#include <compiler/visitors/generation.hpp>
#include <compiler/symbols.hpp>
#endif

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

            // Check for --warnings-as-errors, or -w.
            if (CLI::has_parameter("warnings-as-errors") || CLI::has_flag('w'))
            {
                ApplicationParameters::runtime_warnings_as_errors = true;
                std::cout << "Runtime warnings will be treated as errors." << std::endl;
            }

            Filepath runtime_path = file_get_runtime_directory();
            Filepath output_path = file_get_current_working_directory();
            output_path += "/output";
            output_path.canonicalize();

            ApplicationParameters::runtime_path = runtime_path.c_str();
            ApplicationParameters::output_path = output_path.c_str();

            printf("-- Runtime Path: %s\n", ApplicationParameters::runtime_path.c_str());
            printf("-- Output Path: %s\n", ApplicationParameters::output_path.c_str());
            printf("-- Source File: %s\n", user_source_file.c_str());

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


    // --- Runtime Statistics --------------------------------------------------
    //
    // Displays the runtime statistics.
    //

    u64 total_allocated     = ApplicationParameters::Allocator.get_total_allocated();
    u64 total_released      = ApplicationParameters::Allocator.get_total_released();
    u64 current_allocated   = ApplicationParameters::Allocator.get_current_allocated();
    u64 peak_allocated      = ApplicationParameters::Allocator.get_peak_allocated();

    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "              Runtime Statistics" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "  Memory" << std::endl;
    std::cout << "      Total Allocated:    " << total_allocated << " bytes." << std::endl;
    std::cout << "      Total Released:     " << total_released << " bytes." << std::endl;
    std::cout << "      Current Allocated:  " << current_allocated << " bytes." << std::endl;
    std::cout << "      Peak Allocated:     " << peak_allocated << " bytes." << std::endl;

    return 0;

}
