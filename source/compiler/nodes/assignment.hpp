#ifndef SIGMAFOX_COMPILER_NODES_ASSIGNMENT_HPP
#define SIGMAFOX_COMPILER_NODES_ASSIGNMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeAssignment : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeAssignment();
        inline virtual         ~SyntaxNodeAssignment();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> left;
        shared_ptr<ISyntaxNode> right;

};

SyntaxNodeAssignment::
SyntaxNodeAssignment()
{

    this->type = SyntaxNodeType::NodeTypeAssignment;

}

SyntaxNodeAssignment::
~SyntaxNodeAssignment()
{

}

void SyntaxNodeAssignment::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeAssignment(this);

}

#endif

