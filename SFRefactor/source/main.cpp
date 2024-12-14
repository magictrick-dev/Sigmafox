#include <iostream>
#include <iomanip>
#include <string>
#include <environment.hpp>
#include <platform/filesystem.hpp>
#include <platform/system.hpp>
#include <utilities/path.hpp>
#include <utilities/cli.hpp>
#include <compiler/tokenizer.hpp>
#include <compiler/parser.hpp>
#include <compiler/syntaxtree.hpp>
#include <compiler/visitors/reference.hpp>
#include <compiler/symbols.hpp>

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

    Symboltable<Symbol, FNV1A32Hash, 0.80f> hash_table;
    hash_table.insert("Hello", {
        .identifier = "Hello",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 0,
    });

    if (hash_table.contains("Hello"))
    {
        std::cout << "Does (and should) contain 'Hello'." << std::endl;
    }

    if (!hash_table.contains("Hello World"))
    {
        std::cout << "Doesn't (and shouldn't) contain 'Hello World'." << std::endl;
    }
 
    /*
    SyntaxTree syntax_tree;
    if (!syntax_tree.construct_ast(user_source_file))
    {
       return -1;
    }
    else
    {
        ReferenceVisitor reference_visitor(4);
        syntax_tree.visit_root(&reference_visitor);
        syntax_tree.dump_dependency_graph();
    }
    */


    return 0;

}
