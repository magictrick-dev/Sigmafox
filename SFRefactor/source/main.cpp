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

    // Set the table up.
    Symboltable<Symbol, MurmurHash64A<0x1>, 0.50f> hash_table;

    // Test the table.
    hash_table.insert("Hello", {
        .identifier = "Hello",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 0,
    });

    hash_table.insert("World", {
        .identifier = "World",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 1,
    });

    hash_table.insert("Foo", {
        .identifier = "Foo",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 2,
    });

    std::cout   << "Symbol table is at: " << hash_table.commit() << "/" 
                << hash_table.size() << std::endl;
    std::cout   << "Total overlaps (insert misses): " << hash_table.overlaps() << std::endl;

    hash_table.insert("Bar", {
        .identifier = "Bar",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 3,
    });

    std::cout   << "Symbol table is at: " << hash_table.commit() << "/" 
                << hash_table.size() << std::endl;
    std::cout   << "Total overlaps (insert misses): " << hash_table.overlaps() << std::endl;

    hash_table.insert("Baz", {
        .identifier = "Baz",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 3,
    });

    std::cout   << "Symbol table is at: " << hash_table.commit() << "/" 
                << hash_table.size() << std::endl;
    std::cout   << "Total overlaps (insert misses): " << hash_table.overlaps() << std::endl;

    if (hash_table.contains("Hello")) std::cout << "Contains 'Hello'." << std::endl;
    if (hash_table.contains("World")) std::cout << "Contains 'World'." << std::endl;
    if (hash_table.contains("Foo")) std::cout << "Contains 'Foo'." << std::endl;
    if (hash_table.contains("Bar")) std::cout << "Contains 'Bar'." << std::endl;
    if (hash_table.contains("Baz")) std::cout << "Contains 'Baz'." << std::endl;

    hash_table.insert("Baq", {
        .identifier = "Baq",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 4,
    });

    hash_table.insert("Bac", {
        .identifier = "Bac",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 5,
    });

    hash_table.insert("Ban", {
        .identifier = "Ban",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 6,
    });

    hash_table.insert("Bap", {
        .identifier = "Bap",
        .type       = SymType::SYMBOL_TYPE_VARIABLE,
        .arity      = 7,
    });

    std::cout   << "Symbol table is at: " << hash_table.commit() << "/" 
                << hash_table.size() << std::endl;
    std::cout   << "Total overlaps (insert misses): " << hash_table.overlaps() << std::endl;
 
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
