#include <iostream>
#include <string>
#include <environment.h>
#include <platform/filesystem.h>
#include <platform/system.h>
#include <utilities/path.h>
#include <utilities/cli.h>
#include <utilities/resourcemanager.h>
#include <compiler/tokenizer.h>

using namespace Sigmafox;

int
main(int argc, char ** argv)
{

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

    std::cout << "User Provided Source: " << user_source_file << std::endl;

    Tokenizer entry_tokenizer(user_source_file);

    return 0;

}
