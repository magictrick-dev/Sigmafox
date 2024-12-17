#ifndef SIGMAFOX_COMPILER_NODES_UNARY_HPP
#define SIGMAFOX_COMPILER_NODES_UNARY_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeUnary : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeUnary();
        inline virtual         ~SyntaxNodeUnary();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> right;
        TokenType               operation_type;

};

SyntaxNodeUnary::
SyntaxNodeUnary()
{

    this->type = SyntaxNodeType::NodeTypeUnary;

}

SyntaxNodeUnary::
~SyntaxNodeUnary()
{

}

void SyntaxNodeUnary::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeUnary(this);

}

#endif

