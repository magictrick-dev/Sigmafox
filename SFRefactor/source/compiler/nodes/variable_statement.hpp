#ifndef SIGMAFOX_COMPILER_NODES_VARIABLE_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_VARIABLE_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

enum class VariableType
{
    VariableTypeUndefined,
    VariableTypeInteger,
    VariableTypeFloat,
    VariableTypeString,
    VariableTypeBoolean,
    VariableTypeArray,
};

class SyntaxNodeVariableStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeVariableStatement();
        inline virtual         ~SyntaxNodeVariableStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::string     variable_name;
        VariableType    variable_type;

        shared_ptr<ISyntaxNode> right_hand_side;

};

SyntaxNodeVariableStatement::
SyntaxNodeVariableStatement()
{

    this->type = SyntaxNodeType::NodeTypeVariableStatement;

}

SyntaxNodeVariableStatement::
~SyntaxNodeVariableStatement()
{

}

void SyntaxNodeVariableStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeVariableStatement(this);

}


#endif
