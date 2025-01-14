#ifndef SIGMAFOX_COMPILER_NODES_WRITE_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_WRITE_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeWriteStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeWriteStatement();
        inline virtual         ~SyntaxNodeWriteStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> unit;
        std::vector<shared_ptr<ISyntaxNode>> expressions;

};

SyntaxNodeWriteStatement::
SyntaxNodeWriteStatement()
{

    this->type = SyntaxNodeType::NodeTypeWriteStatement;

}

SyntaxNodeWriteStatement::
~SyntaxNodeWriteStatement()
{

}

void SyntaxNodeWriteStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeWriteStatement(this);

}

#endif
