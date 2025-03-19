#ifndef SIGMAFOX_COMPILER_NODES_TERM_HPP
#define SIGMAFOX_COMPILER_NODES_TERM_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeTerm : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeTerm();
        inline virtual         ~SyntaxNodeTerm();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> left;
        shared_ptr<ISyntaxNode> right;
        TokenType               operation_type;

};

SyntaxNodeTerm::
SyntaxNodeTerm()
{

    this->type = SyntaxNodeType::NodeTypeTerm;

}

SyntaxNodeTerm::
~SyntaxNodeTerm()
{

}

void SyntaxNodeTerm::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeTerm(this);

}

#endif

