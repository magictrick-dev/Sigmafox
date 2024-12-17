#ifndef SIGMAFOX_COMPILER_NODES_PRIMARY_HPP
#define SIGMAFOX_COMPILER_NODES_PRIMARY_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeGrouping : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeGrouping();
        inline virtual         ~SyntaxNodeGrouping();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> expression;

};

SyntaxNodeGrouping::
SyntaxNodeGrouping()
{

    this->type = SyntaxNodeType::NodeTypeGrouping;

}

SyntaxNodeGrouping::
~SyntaxNodeGrouping()
{

}

void SyntaxNodeGrouping::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeGrouping(this);

}

#endif

