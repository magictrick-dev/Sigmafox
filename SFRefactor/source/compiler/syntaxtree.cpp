#include <compiler/syntaxtree.hpp>

SyntaxTree::
SyntaxTree()
{

}

SyntaxTree::
~SyntaxTree()
{

}

bool SyntaxTree::
construct_ast(Filepath entry_file)
{

    return false;
}

void SyntaxTree::
visit_root(ISyntaxNodeVisitor *visitor)
{

    SF_ENSURE_PTR(visitor);
    SF_ENSURE_PTR(this->root);
    this->root->accept(visitor);

}
