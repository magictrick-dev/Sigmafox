#include <compiler/compiler.hpp>
#include <compiler/parser/parser.hpp>

Compiler::
Compiler(string entry_file)
{

    this->graph.set_root(entry_file);

}

Compiler::
~Compiler()
{

}

bool Compiler::
parse()
{

    // Generate the parser.
    ParseTree parser(&this->graph, &this->environment);

    // Parse the entry file.
    parser.parse(this->graph.get_root_path());
    if (!parser.valid())
        return false;

    this->root = parser.get_root();
    return true;

}

bool Compiler::
validate() const
{

    if (root == nullptr) return false;
    return true;

}

bool Compiler::
generate() const
{

    return true;

}