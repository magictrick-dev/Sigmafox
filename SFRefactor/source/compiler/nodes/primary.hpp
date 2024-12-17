#ifndef SIGMAFOX_COMPILER_NODES_PRIMARY_HPP
#define SIGMAFOX_COMPILER_NODES_PRIMARY_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodePrimary : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodePrimary();
        inline virtual         ~SyntaxNodePrimary();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> expression;

};

SyntaxNodePrimary::
SyntaxNodePrimary()
{

    this->type = SyntaxNodeType::NodeTypePrimary;

}

SyntaxNodePrimary::
~SyntaxNodePrimary()
{

}

void SyntaxNodePrimary::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodePrimary(this);

}

#endif

