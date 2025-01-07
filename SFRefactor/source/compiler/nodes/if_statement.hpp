#ifndef SIGMAFOX_COMPILER_NODES_IF_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_IF_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeIfStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeIfStatement();
        inline virtual         ~SyntaxNodeIfStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> conditional;
        shared_ptr<ISyntaxNode> conditional_else;
        std::vector<shared_ptr<ISyntaxNode>> children;

};

SyntaxNodeIfStatement::
SyntaxNodeIfStatement()
{

    this->type = SyntaxNodeType::NodeTypeIfStatement;

}

SyntaxNodeIfStatement::
~SyntaxNodeIfStatement()
{

}

void SyntaxNodeIfStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeIfStatement(this);

}

#endif