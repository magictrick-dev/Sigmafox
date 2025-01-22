#ifndef SIGMAFOX_COMPILER_VISITORS_EXPRESSIONS_HPP
#define SIGMAFOX_COMPILER_VISITORS_EXPRESSIONS_HPP
#include <compiler/syntaxvisitor.hpp>
#include <compiler/syntaxnodes.hpp>
#include <vector>
#include <stack>

class ExpressionVisitor : public ISyntaxNodeVisitor
{

    public:
                        ExpressionVisitor();   
        virtual        ~ExpressionVisitor();

    public:
        virtual void visit_SyntaxNodeExpression(SyntaxNodeExpression *node) override;     
        virtual void visit_SyntaxNodeProcedureCall(SyntaxNodeProcedureCall *node) override;
        virtual void visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node) override;     
        virtual void visit_SyntaxNodeEquality(SyntaxNodeEquality *node) override;         
        virtual void visit_SyntaxNodeComparison(SyntaxNodeComparison *node) override;     
        virtual void visit_SyntaxNodeTerm(SyntaxNodeTerm *node) override;                 
        virtual void visit_SyntaxNodeFactor(SyntaxNodeFactor *node) override;             
        virtual void visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node) override;       
        virtual void visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node) override;     
        virtual void visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node) override;     
        virtual void visit_SyntaxNodeUnary(SyntaxNodeUnary *node) override;               
        virtual void visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node) override; 
        virtual void visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node) override; 
        virtual void visit_SyntaxNodePrimary(SyntaxNodePrimary *node) override;           
        virtual void visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node) override;

};


#endif