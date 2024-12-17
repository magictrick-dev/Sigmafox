#ifndef SIGMAFOX_COMPILER_NODES_EXTRACTION_HPP
#define SIGMAFOX_COMPILER_NODES_EXTRACTION_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeExtraction : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeExtraction();
        inline virtual         ~SyntaxNodeExtraction();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> left;
        shared_ptr<ISyntaxNode> right;
        TokenType               operation_type;

};

SyntaxNodeExtraction::
SyntaxNodeExtraction()
{

    this->type = SyntaxNodeType::NodeTypeExtraction;

}

SyntaxNodeExtraction::
~SyntaxNodeExtraction()
{

}

void SyntaxNodeExtraction::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeExtraction(this);

}

#endif

