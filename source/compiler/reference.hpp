#ifndef SIGMAFOX_COMPILER_REFERENCE_HPP
#define SIGMAFOX_COMPILER_REFERENCE_HPP
#include <definitions.hpp>
#include <compiler/parser/visitor.hpp>

class ReferenceVisitor : public SyntaxNodeVisitor
{
    public:
                        ReferenceVisitor();
                        ReferenceVisitor(i32 tab_size);
        virtual        ~ReferenceVisitor();

        virtual void    visit(SyntaxNodeRoot* node) override;
        virtual void    visit(SyntaxNodeModule* node) override;
        virtual void    visit(SyntaxNodeMain* node) override;
        virtual void    visit(SyntaxNodeIncludeStatement* node) override;
        virtual void    visit(SyntaxNodeParameter* node) override;
        virtual void    visit(SyntaxNodeFunctionStatement* node) override;
        virtual void    visit(SyntaxNodeProcedureStatement* node) override;
        virtual void    visit(SyntaxNodeExpressionStatement* node) override;
        virtual void    visit(SyntaxNodeProcedureCallStatement* node) override;
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
        void print_tabs();
        void push_tabs();
        void pop_tabs();

    protected:
        i32 tabs        = 0;
        i32 tab_size    = 4;

};


#endif