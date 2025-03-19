#ifndef SIGMAFOX_COMPILER_NODES_VARIABLE_STATEMENT_HPP
#define SIGMAFOX_COMPILER_NODES_VARIABLE_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeVariableStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeVariableStatement();
        inline virtual         ~SyntaxNodeVariableStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::string variable_name;
        std::string variable_type;
        shared_ptr<ISyntaxNode> right_hand_side;

        shared_ptr<ISyntaxNode> size;
        std::vector<shared_ptr<ISyntaxNode>> dimensions;

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
