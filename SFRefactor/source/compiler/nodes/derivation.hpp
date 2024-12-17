#ifndef SIGMAFOX_COMPILER_NODES_EXTRACTION_HPP
#define SIGMAFOX_COMPILER_NODES_EXTRACTION_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeDerivation : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeDerivation();
        inline virtual         ~SyntaxNodeDerivation();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> expression;

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

