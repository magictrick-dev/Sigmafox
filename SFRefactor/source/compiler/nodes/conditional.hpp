#ifndef SIGMAFOX_COMPILER_NODES_CONDITIONAL_HPP
#define SIGMAFOX_COMPILER_NODES_CONDITIONAL_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeConditional : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeConditional();
        inline virtual         ~SyntaxNodeConditional();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> condition;
        shared_ptr<ISyntaxNode> conditional_else;
        std::vector<shared_ptr<ISyntaxNode>> children;

};

SyntaxNodeConditional::
SyntaxNodeConditional()
{

    this->type = SyntaxNodeType::NodeTypeConditional;

}

SyntaxNodeConditional::
~SyntaxNodeConditional()
{

}

void SyntaxNodeConditional::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeConditional(this);

}

#endif