#ifndef SIGMAFOX_COMPILER_PARSER_VALIDATORS_BLOCKVALIDATOR_HPP
#define SIGMAFOX_COMPILER_PARSER_VALIDATORS_BLOCKVALIDATOR_HPP
#include <definitions.hpp>
#include <compiler/environment.hpp>
#include <compiler/parser/visitor.hpp>

class BlockValidator : public SyntaxNodeVisitor
{
    public:
                        BlockValidator(Environment *environment);
        virtual        ~BlockValidator();
        
        virtual void    visit(SyntaxNodeFunctionStatement* node) override;
        virtual void    visit(SyntaxNodeProcedureStatement* node) override;
        virtual void    visit(SyntaxNodeExpressionStatement* node) override;
        virtual void    visit(SyntaxNodeProcedureCall* node) override;
        virtual void    visit(SyntaxNodeWhileStatement* node) override;
        virtual void    visit(SyntaxNodeLoopStatement* node) override;
        virtual void    visit(SyntaxNodeVariableStatement* node) override;
        virtual void    visit(SyntaxNodeScopeStatement* node) override;
        virtual void    visit(SyntaxNodeConditionalStatement* node) override;
        virtual void    visit(SyntaxNodeReadStatement* node) override;
        virtual void    visit(SyntaxNodeWriteStatement* node) override;

        virtual void    visit(SyntaxNodeExpression* node) override;
        virtual void    visit(SyntaxNodeAssignment* node) override;
        virtual void    visit(SyntaxNodeEquality* node) override;
        virtual void    visit(SyntaxNodeComparison* node) override;
        virtual void    visit(SyntaxNodeTerm* node) override;
        virtual void    visit(SyntaxNodeFactor* node) override;
        virtual void    visit(SyntaxNodeMagnitude* node) override;
        virtual void    visit(SyntaxNodeExtraction* node) override;
        virtual void    visit(SyntaxNodeDerivation* node) override;
        virtual void    visit(SyntaxNodeUnary* node) override;
        virtual void    visit(SyntaxNodeFunctionCall* node) override;
        virtual void    visit(SyntaxNodeArrayIndex* node) override;
        virtual void    visit(SyntaxNodePrimary* node) override;
        virtual void    visit(SyntaxNodeGrouping* node) override;
    
        
        
    protected:
        Environment            *environment;
        std::vector<string>     call_stack;

};

#endif
