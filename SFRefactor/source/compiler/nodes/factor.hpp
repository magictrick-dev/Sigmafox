#ifndef SIGMAFOX_COMPILER_NODES_FACTOR_HPP
#define SIGMAFOX_COMPILER_NODES_FACTOR_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeFactor : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeFactor();
        inline virtual         ~SyntaxNodeFactor();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> left;
        shared_ptr<ISyntaxNode> right;
        TokenType               operation_type;

};

SyntaxNodeFactor::
SyntaxNodeFactor()
{

    this->type = SyntaxNodeType::NodeTypeFactor;

}

SyntaxNodeFactor::
~SyntaxNodeFactor()
{

}

void SyntaxNodeFactor::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeFactor(this);

}

#endif

