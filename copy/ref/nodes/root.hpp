#ifndef SIGMAFOX_COMPILER_NODES_ROOT_HPP
#define SIGMAFOX_COMPILER_NODES_ROOT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeRoot : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeRoot();
        inline virtual         ~SyntaxNodeRoot();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::vector<shared_ptr<ISyntaxNode>> globals;
        shared_ptr<ISyntaxNode> main;

};

SyntaxNodeRoot::
SyntaxNodeRoot()
{

    this->type = SyntaxNodeType::NodeTypeRoot;

}

SyntaxNodeRoot::
~SyntaxNodeRoot()
{

}

void SyntaxNodeRoot::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeRoot(this);

}

#endif
