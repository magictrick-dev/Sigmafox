#ifndef SIGMAFOX_COMPILER_NODES_EXPRESSION_HPP
#define SIGMAFOX_COMPILER_NODES_EXPRESSION_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeExpression : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeExpression();
        inline virtual         ~SyntaxNodeExpression();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> expression;

};

SyntaxNodeExpression::
SyntaxNodeExpression()
{

    this->type = SyntaxNodeType::NodeTypeExpression;

}

SyntaxNodeExpression::
~SyntaxNodeExpression()
{

}

void SyntaxNodeExpression::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeExpression(this);

}

#endif
