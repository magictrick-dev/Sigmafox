#ifndef SIGMAFOX_COMPILER_NODES_PROCEDURE_CALL_HPP
#define SIGMAFOX_COMPILER_NODES_PROCEDURE_CALL_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeProcedureCall : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeProcedureCall();
        inline virtual         ~SyntaxNodeProcedureCall();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::string procedure_name;
        std::vector<shared_ptr<ISyntaxNode>> parameters;

};

SyntaxNodeProcedureCall::
SyntaxNodeProcedureCall()
{

    this->type = SyntaxNodeType::NodeTypeProcedureCall;

}

SyntaxNodeProcedureCall::
~SyntaxNodeProcedureCall()
{

}

void SyntaxNodeProcedureCall::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeProcedureCall(this);

}



#endif