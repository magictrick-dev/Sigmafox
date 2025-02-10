#ifndef SIGMAFOX_COMPILER_NODES_WHILE_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_WHILE_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeWhileStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeWhileStatement();
        inline virtual         ~SyntaxNodeWhileStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> condition;
        std::vector<shared_ptr<ISyntaxNode>> children;

};

SyntaxNodeWhileStatement::
SyntaxNodeWhileStatement()
{

    this->type = SyntaxNodeType::NodeTypeWhileStatement;

}

SyntaxNodeWhileStatement::
~SyntaxNodeWhileStatement()
{

}

void SyntaxNodeWhileStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeWhileStatement(this);

}






#endif