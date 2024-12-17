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
        shared_ptr<ISyntaxNode> expression;

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

