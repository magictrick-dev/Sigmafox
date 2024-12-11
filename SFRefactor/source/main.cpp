#include <iostream>
#include <iomanip>
#include <string>
#include <environment.hpp>
#include <platform/filesystem.hpp>
#include <platform/system.hpp>
#include <utilities/path.hpp>
#include <utilities/cli.hpp>
#include <utilities/resourcemanager.hpp>
#include <compiler/tokenizer.hpp>
#include <compiler/parser.hpp>
#include <compiler/syntaxtree.hpp>

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
    
    SyntaxTree syntax_tree;
    if (!syntax_tree.construct_ast(user_source_file))
    {
        std::cout << "Unable to construct AST." << std::endl;
    }
    else
    {
        std::cout << "Successfully construct AST." << std::endl;
    }

    // Create the root parser.
    //SyntaxParser root_parser(user_source_file);

    // Resolve the dependencies.
    //DependencyResolver resolver(&root_parser);
    //bool resolution = resolver.resolve();

#if 0
    Tokenizer entry_tokenizer(user_source_file);
    Token current_token = entry_tokenizer.get_current_token();
    while (current_token.type != TokenType::TOKEN_EOF)
    {
        std::cout   << std::setw(32) << current_token.type_to_string() << " : "
                    << current_token.to_string() << std::endl;
        
        entry_tokenizer.shift();
        current_token = entry_tokenizer.get_current_token();
    }
#endif

    return 0;

}
