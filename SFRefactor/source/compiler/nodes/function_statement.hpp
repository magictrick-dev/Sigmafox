#ifndef SIGMAFOX_COMPILER_NODES_FUNCTION_CALL_HPP
#define SIGMAFOX_COMPILER_NODES_FUNCTION_CALL_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeFunctionStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeFunctionStatement();
        inline virtual         ~SyntaxNodeFunctionStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::string identifier_name;
        std::vector<std::string> parameters;
        std::vector<shared_ptr<ISyntaxNode>> body_statements;

};

SyntaxNodeFunctionStatement::
SyntaxNodeFunctionStatement()
{

    this->type = SyntaxNodeType::NodeTypeFunctionStatement;

}

SyntaxNodeFunctionStatement::
~SyntaxNodeFunctionStatement()
{

}

void SyntaxNodeFunctionStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeFunctionStatement(this);

}



#endif