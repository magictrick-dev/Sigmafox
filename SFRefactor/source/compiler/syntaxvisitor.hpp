#ifndef SIGMAFOX_COMPILER_SYNTAX_VISITOR_HPP
#define SIGMAFOX_COMPILER_SYNTAX_VISITOR_HPP

class SyntaxNodeRoot;
class SyntaxNodeModule;

class SyntaxNodeInclude;

class SyntaxNodeMain;
class SyntaxNodeExpressionStatement;
class SyntaxNodeScopeStatement;
class SyntaxNodeWhileStatement;
class SyntaxNodeLoopStatement;
class SyntaxNodeVariableStatement;
class SyntaxNodeFunctionStatement;
class SyntaxNodeProcedureStatement;
class SyntaxNodeIfStatement;
class SyntaxNodeConditional;
class SyntaxNodeReadStatement;
class SyntaxNodeWriteStatement;

class SyntaxNodeExpression;
class SyntaxNodeProcedureCall;
class SyntaxNodeAssignment;
class SyntaxNodeEquality;
class SyntaxNodeComparison;
class SyntaxNodeTerm;
class SyntaxNodeFactor;
class SyntaxNodeMagnitude;
class SyntaxNodeExtraction;
class SyntaxNodeDerivation;
class SyntaxNodeUnary;
class SyntaxNodeFunctionCall;
class SyntaxNodeArrayIndex;
class SyntaxNodePrimary;
class SyntaxNodeGrouping;

// Provides an easy way to traverse the AST in a uniform fashion without having to
// extend any internal behaviors on the nodes themselves. They automatically recurse
// on their child nodes.
class ISyntaxNodeVisitor
{

    public:
        inline virtual void visit_SyntaxNodeRoot(SyntaxNodeRoot *node) { }
        inline virtual void visit_SyntaxNodeModule(SyntaxNodeModule *node) { }
        inline virtual void visit_SyntaxNodeMain(SyntaxNodeMain *node) { }

        inline virtual void visit_SyntaxNodeInclude(SyntaxNodeInclude *node) { }
        inline virtual void visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node) { }
        inline virtual void visit_SyntaxNodeWhileStatement(SyntaxNodeWhileStatement *node) { }
        inline virtual void visit_SyntaxNodeLoopStatement(SyntaxNodeLoopStatement *node) { }
        inline virtual void visit_SyntaxNodeVariableStatement(SyntaxNodeVariableStatement *node) { }
        inline virtual void visit_SyntaxNodeScopeStatement(SyntaxNodeScopeStatement *node) { }
        inline virtual void visit_SyntaxNodeFunctionStatement(SyntaxNodeFunctionStatement *node) { }
        inline virtual void visit_SyntaxNodeProcedureStatement(SyntaxNodeProcedureStatement *node) { }
        inline virtual void visit_SyntaxNodeIfStatement(SyntaxNodeIfStatement *node) { }
        inline virtual void visit_SyntaxNodeConditional(SyntaxNodeConditional *node) { }
        inline virtual void visit_SyntaxNodeReadStatement(SyntaxNodeReadStatement *node) { }
        inline virtual void visit_SyntaxNodeWriteStatement(SyntaxNodeWriteStatement *node) { }

        inline virtual void visit_SyntaxNodeExpression(SyntaxNodeExpression *node) { }
        inline virtual void visit_SyntaxNodeProcedureCall(SyntaxNodeProcedureCall *node) { }
        inline virtual void visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node) { }
        inline virtual void visit_SyntaxNodeEquality(SyntaxNodeEquality *node) { }
        inline virtual void visit_SyntaxNodeComparison(SyntaxNodeComparison *node) { }
        inline virtual void visit_SyntaxNodeTerm(SyntaxNodeTerm *node) { }
        inline virtual void visit_SyntaxNodeFactor(SyntaxNodeFactor *node) { }
        inline virtual void visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node) { }
        inline virtual void visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node) { }
        inline virtual void visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node) { }
        inline virtual void visit_SyntaxNodeUnary(SyntaxNodeUnary *node) { }
        inline virtual void visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node) { }
        inline virtual void visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node) { }
        inline virtual void visit_SyntaxNodePrimary(SyntaxNodePrimary *node) { }
        inline virtual void visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node) { }

};

#endif