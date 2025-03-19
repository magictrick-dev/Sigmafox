#ifndef SIGMAFOX_COMPILER_NODES_PROCEDURE_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_PROCEDURE_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeProcedureStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeProcedureStatement();
        inline virtual         ~SyntaxNodeProcedureStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::string identifier_name;
        std::vector<std::string> parameters;
        std::vector<shared_ptr<ISyntaxNode>> body_statements;

};

SyntaxNodeProcedureStatement::
SyntaxNodeProcedureStatement()
{

    this->type = SyntaxNodeType::NodeTypeProcedureStatement;

}

SyntaxNodeProcedureStatement::
~SyntaxNodeProcedureStatement()
{

}

void SyntaxNodeProcedureStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeProcedureStatement(this);

}

#endif
