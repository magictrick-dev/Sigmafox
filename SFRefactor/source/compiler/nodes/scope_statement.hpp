#ifndef SIGMAFOX_COMPILER_SCOPE_STATEMENT_HPP
#define SIGMAFOX_COMPILER_SCOPE_STATEMENT_HPP
#include <compiler/syntaxnode.hpp>

class SyntaxNodeScopeStatement : public ISyntaxNode 
{

    public:
        inline                  SyntaxNodeScopeStatement();
        inline virtual         ~SyntaxNodeScopeStatement();
        inline virtual void     accept(ISyntaxNodeVisitor *visitor) override;

    public:
        std::vector<shared_ptr<ISyntaxNode>> children;

};

SyntaxNodeScopeStatement::
SyntaxNodeScopeStatement()
{

    this->type = SyntaxNodeType::NodeTypeScopeStatement;

}

SyntaxNodeScopeStatement::
~SyntaxNodeScopeStatement()
{

}

void SyntaxNodeScopeStatement::
accept(ISyntaxNodeVisitor *visitor)
{

    visitor->visit_SyntaxNodeScopeStatement(this);

}



#endif
