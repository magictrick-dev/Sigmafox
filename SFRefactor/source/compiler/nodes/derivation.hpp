#ifndef SIGMAFOX_COMPILER_NODES_DERIVATION_HPP
#define SIGMAFOX_COMPILER_NODES_DERIVATION_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeDerivation : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeDerivation();
        inline virtual         ~SyntaxNodeDerivation();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> left;
        shared_ptr<ISyntaxNode> right;
        TokenType               operation_type;

};

SyntaxNodeDerivation::
SyntaxNodeDerivation()
{

    this->type = SyntaxNodeType::NodeTypeDerivation;

}

SyntaxNodeDerivation::
~SyntaxNodeDerivation()
{

}

void SyntaxNodeDerivation::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeDerivation(this);

}

#endif

