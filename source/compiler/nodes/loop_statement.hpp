#ifndef SIGMAFOX_COMPILER_NODES_LOOP_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_LOOP_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeLoopStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeLoopStatement();
        inline virtual         ~SyntaxNodeLoopStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::string identifier;
        shared_ptr<ISyntaxNode> initial;
        shared_ptr<ISyntaxNode> terminal;
        shared_ptr<ISyntaxNode> step;
        std::vector<shared_ptr<ISyntaxNode>> children;

};

SyntaxNodeLoopStatement::
SyntaxNodeLoopStatement()
{

    this->type = SyntaxNodeType::NodeTypeLoopStatement;

}

SyntaxNodeLoopStatement::
~SyntaxNodeLoopStatement()
{

}

void SyntaxNodeLoopStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeLoopStatement(this);

}


#endif