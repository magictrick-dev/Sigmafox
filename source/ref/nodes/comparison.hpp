#ifndef SIGMAFOX_COMPILER_NODES_COMPARISON_HPP
#define SIGMAFOX_COMPILER_NODES_COMPARISON_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeComparison : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeComparison();
        inline virtual         ~SyntaxNodeComparison();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> left;
        shared_ptr<ISyntaxNode> right;
        TokenType               operation_type;

};

SyntaxNodeComparison::
SyntaxNodeComparison()
{

    this->type = SyntaxNodeType::NodeTypeComparison;

}

SyntaxNodeComparison::
~SyntaxNodeComparison()
{

}

void SyntaxNodeComparison::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeComparison(this);

}

#endif

