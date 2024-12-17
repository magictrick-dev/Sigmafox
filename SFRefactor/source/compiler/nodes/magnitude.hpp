#ifndef SIGMAFOX_COMPILER_NODES_MAGNITUDE_HPP
#define SIGMAFOX_COMPILER_NODES_MAGNITUDE_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeMagnitude : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeMagnitude();
        inline virtual         ~SyntaxNodeMagnitude();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> expression;

};

SyntaxNodeMagnitude::
SyntaxNodeMagnitude()
{

    this->type = SyntaxNodeType::NodeTypeMagnitude;

}

SyntaxNodeMagnitude::
~SyntaxNodeMagnitude()
{

}

void SyntaxNodeMagnitude::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeMagnitude(this);

}

#endif

