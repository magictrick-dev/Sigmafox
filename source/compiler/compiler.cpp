#include <compiler/compiler.hpp>
#include <compiler/reference.hpp>
#include <compiler/parser/parser.hpp>
#include <compiler/generation/generator.hpp>

Compiler::
Compiler(string entry_file)
{

    std::cout << "Root file is: " << entry_file.c_str() << std::endl;
    this->graph.set_root(entry_file);

}

Compiler::
~Compiler()
{
    
    //for (auto node : this->nodes) delete node;

}

bool Compiler::
parse()
{

    // Generate the parser.
    ParseTree parser(&this->graph, &this->environment);

    // Parse the entry file.
    parser.parse(this->graph.get_root_path());

    // If the parser didn't return a valid tree back, return false.
    if (!parser.valid())
    {
        return false;
    }

    // If the parser cascaded into an error at any point, the tree can't be used
    // for generation. Return false.
    if (!this->environment.is_valid_parse())
    {
        return false;
    }

    this->root = parser.get_root();
    
    // Move the nodes out so when the parser goes out of scope it doesn't
    // clear our nodes with it.
    std::vector<shared_ptr<SyntaxNode>>& nodes = parser.get_nodes();
    this->nodes.insert(this->nodes.end(), 
        std::make_move_iterator(nodes.begin()),
        std::make_move_iterator(nodes.end()));
    nodes.clear();

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

#if 0
    // Tests and outputs the AST.
    ReferenceVisitor visitor;
    this->root->accept(&visitor);

#else
    ReferenceVisitor visitor;
    this->root->accept(&visitor);

    TranspileCPPGenerator generator;
    this->root->accept(&generator);
#endif

    return true;

}
