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

    // Set the table up. The following are some hashing options available to use.
    // Standard hash is what unordered_map uses by default, while FNV1A32/64 and Murmur
    // are some of the popular hashing algorithms. Murmur<> defaults the seed to
    // 0xFFFF'FFFF'FFFF'FFFF, however 0x1 seems to perform pretty good.
    //Symboltable<Symbol, std::hash<std::string>, 0.50f> hash_table;
    //Symboltable<Symbol, FNV1A32, 0.50f> hash_table;
    //Symboltable<Symbol, FNV1A64, 0.50f> hash_table;
    //Symboltable<Symbol, Murmur64A<>, 0.50f> hash_table;
    Symboltable<Symbol, Murmur64A<0x1>, 0.80f> table_a;
    Symboltable<Symbol, Murmur64A<0x1>, 0.80f> table_b;

    table_a.insert("hello",     { .identifier = "hello"     });
    table_a.insert("bar",       { .identifier = "bar"       });
    table_b.insert("world",     { .identifier = "world"     });
    table_b.insert("foo",       { .identifier = "foo"       });

    if (table_a.contains("hello")) std::cout << "Table A: Contains 'hello'" << std::endl;
    if (table_a.contains("world")) std::cout << "Table A: Contains 'world'" << std::endl;
    if (table_a.contains("foo")) std::cout << "Table A: Contains 'foo'" << std::endl;
    if (table_a.contains("bar")) std::cout << "Table A: Contains 'bar'" << std::endl;
    std::cout << std::endl;
    if (table_b.contains("hello")) std::cout << "Table B: Contains 'hello'" << std::endl;
    if (table_b.contains("world")) std::cout << "Table B: Contains 'world'" << std::endl;
    if (table_b.contains("foo")) std::cout << "Table B: Contains 'foo'" << std::endl;
    if (table_b.contains("bar")) std::cout << "Table B: Contains 'bar'" << std::endl;
    std::cout << std::endl;

    table_b.merge_from(table_a);

    if (table_b.contains("hello")) std::cout << "Table B: Contains 'hello'" << std::endl;
    if (table_b.contains("world")) std::cout << "Table B: Contains 'world'" << std::endl;
    if (table_b.contains("foo")) std::cout << "Table B: Contains 'foo'" << std::endl;
    if (table_b.contains("bar")) std::cout << "Table B: Contains 'bar'" << std::endl;
    table_b.get("hello");
    std::cout << "Table B facts: " << table_b.commit() << "/" << table_b.size() << std::endl;
    
    table_b.remove("hello");
    std::cout << std::endl;
    if (table_b.contains("hello")) std::cout << "Table B: Contains 'hello'" << std::endl;
    if (table_b.contains("world")) std::cout << "Table B: Contains 'world'" << std::endl;
    if (table_b.contains("foo")) std::cout << "Table B: Contains 'foo'" << std::endl;
    if (table_b.contains("bar")) std::cout << "Table B: Contains 'bar'" << std::endl;
    std::cout << "Table B facts: " << table_b.commit() << "/" << table_b.size() << std::endl;

    table_b.insert("hello",     { .identifier = "hello"     });
    std::cout << std::endl;
    if (table_b.contains("hello")) std::cout << "Table B: Contains 'hello'" << std::endl;
    if (table_b.contains("world")) std::cout << "Table B: Contains 'world'" << std::endl;
    if (table_b.contains("foo")) std::cout << "Table B: Contains 'foo'" << std::endl;
    if (table_b.contains("bar")) std::cout << "Table B: Contains 'bar'" << std::endl;
    std::cout << "Table B facts: " << table_b.commit() << "/" << table_b.size() << std::endl;

    Symboltable<Symbol, Murmur64A<0x1>, 0.80f> table_c(table_b);
    if (table_c.contains("hello")) std::cout << "Table C: Contains 'hello'" << std::endl;
    if (table_c.contains("world")) std::cout << "Table C: Contains 'world'" << std::endl;
    if (table_c.contains("foo")) std::cout << "Table C: Contains 'foo'" << std::endl;
    if (table_c.contains("bar")) std::cout << "Table C: Contains 'bar'" << std::endl;
    std::cout << "Table C facts: " << table_c.commit() << "/" << table_c.size() << std::endl;

    if (table_a.contains("hello")) std::cout << "Table A: Contains 'hello'" << std::endl;
    if (table_a.contains("world")) std::cout << "Table A: Contains 'world'" << std::endl;
    if (table_a.contains("foo")) std::cout << "Table A: Contains 'foo'" << std::endl;
    if (table_a.contains("bar")) std::cout << "Table A: Contains 'bar'" << std::endl;
    std::cout << std::endl;
    table_a = table_c;
    if (table_a.contains("hello")) std::cout << "Table A: Contains 'hello'" << std::endl;
    if (table_a.contains("world")) std::cout << "Table A: Contains 'world'" << std::endl;
    if (table_a.contains("foo")) std::cout << "Table A: Contains 'foo'" << std::endl;
    if (table_a.contains("bar")) std::cout << "Table A: Contains 'bar'" << std::endl;
    std::cout << std::endl;
    

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
