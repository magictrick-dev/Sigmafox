#include <compiler/syntaxtree.hpp>

SyntaxTree::
SyntaxTree()
{

    this->root = nullptr;

}

SyntaxTree::
~SyntaxTree()
{

}

bool SyntaxTree::
construct_ast(Filepath entry_file)
{

    if (!this->graph.set_entry(entry_file)) return false;
    shared_ptr<SyntaxParser> entry_parser = this->graph.get_parser_for(entry_file);
    if (!entry_parser->construct_as_root()) return false;
    this->root = entry_parser->get_base_node();
    return true;

}

void SyntaxTree::
visit_root(ISyntaxNodeVisitor *visitor)
{

    SF_ENSURE_PTR(visitor);
    SF_ENSURE_PTR(this->root);
    this->root->accept(visitor);

}
