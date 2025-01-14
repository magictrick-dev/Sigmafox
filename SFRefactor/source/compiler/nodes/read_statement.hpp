#ifndef SIGMAFOX_COMPILER_NODES_READ_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_READ_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeReadStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeReadStatement();
        inline virtual         ~SyntaxNodeReadStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> unit;
        std::string identifier;

};

SyntaxNodeReadStatement::
SyntaxNodeReadStatement()
{

    this->type = SyntaxNodeType::NodeTypeReadStatement;

}

SyntaxNodeReadStatement::
~SyntaxNodeReadStatement()
{

}

void SyntaxNodeReadStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeReadStatement(this);

}

#endif

