#ifndef SIGMAFOX_COMPILER_NODES_EQUALITY_HPP
#define SIGMAFOX_COMPILER_NODES_EQUALITY_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeEquality : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeEquality();
        inline virtual         ~SyntaxNodeEquality();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        TokenType               operation_type;
        shared_ptr<ISyntaxNode> left;
        shared_ptr<ISyntaxNode> right;

};

SyntaxNodeEquality::
SyntaxNodeEquality()
{

    this->type = SyntaxNodeType::NodeTypeEquality;

}

SyntaxNodeEquality::
~SyntaxNodeEquality()
{

}

void SyntaxNodeEquality::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeEquality(this);

}

#endif

