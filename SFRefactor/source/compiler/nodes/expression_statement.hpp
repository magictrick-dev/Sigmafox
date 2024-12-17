#ifndef SIGMAFOX_COMPILER_NODES_EXPRESSION_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_EXPRESSION_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeExpressionStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeExpressionStatement();
        inline virtual         ~SyntaxNodeExpressionStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> expression;

};

SyntaxNodeExpressionStatement::
SyntaxNodeExpressionStatement()
{

    this->type = SyntaxNodeType::NodeTypeExpressionStatement;

}

SyntaxNodeExpressionStatement::
~SyntaxNodeExpressionStatement()
{

}

void SyntaxNodeExpressionStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeExpressionStatement(this);

}

#endif
