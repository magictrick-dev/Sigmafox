#ifndef SIGMAFOX_COMPILER_NODES_FUNCTIONCALL_HPP
#define SIGMAFOX_COMPILER_NODES_FUNCTIONCALL_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeFunctionCall : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeFunctionCall();
        inline virtual         ~SyntaxNodeFunctionCall();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<ISyntaxNode> expression;

};

SyntaxNodeFunctionCall::
SyntaxNodeFunctionCall()
{

    this->type = SyntaxNodeType::NodeTypeFunctionCall;

}

SyntaxNodeFunctionCall::
~SyntaxNodeFunctionCall()
{

}

void SyntaxNodeFunctionCall::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeFunctionCall(this);

}

#endif
