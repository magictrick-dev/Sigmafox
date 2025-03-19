#ifndef SIGMAFOX_COMPILER_PARSER_VISITOR_HPP
#define SIGMAFOX_COMPILER_PARSER_VISITOR_HPP
#include <definitions.hpp>
#include <compiler/parser/node.hpp>
#include <compiler/parser/subnodes.hpp>

class SyntaxNodeVisitor
{
    public:
                        SyntaxNodeVisitor();
        virtual        ~SyntaxNodeVisitor();

        virtual void    visit(SyntaxNodeRoot* node);
        virtual void    visit(SyntaxNodeModule* node);
        virtual void    visit(SyntaxNodeMain* node);
        virtual void    visit(SyntaxNodeIncludeStatement* node);
        virtual void    visit(SyntaxNodeFunctionStatement* node);
        virtual void    visit(SyntaxNodeProcedureStatement* node);
        virtual void    visit(SyntaxNodeExpressionStatement* node);
        virtual void    visit(SyntaxNodeWhileStatement* node);
        virtual void    visit(SyntaxNodeLoopStatement* node);
        virtual void    visit(SyntaxNodeVariableStatement* node);
        virtual void    visit(SyntaxNodeScopeStatement* node);
        virtual void    visit(SyntaxNodeConditionalStatement* node);
        virtual void    visit(SyntaxNodeReadStatement* node);
        virtual void    visit(SyntaxNodeWriteStatement* node);
        virtual void    visit(SyntaxNodeExpression* node);
        virtual void    visit(SyntaxNodeProcedureCall* node);
        virtual void    visit(SyntaxNodeAssignment* node);
        virtual void    visit(SyntaxNodeEquality* node);
        virtual void    visit(SyntaxNodeComparison* node);
        virtual void    visit(SyntaxNodeTerm* node);
        virtual void    visit(SyntaxNodeFactor* node);
        virtual void    visit(SyntaxNodeMagnitude* node);
        virtual void    visit(SyntaxNodeExtraction* node);
        virtual void    visit(SyntaxNodeDerivation* node);
        virtual void    visit(SyntaxNodeUnary* node);
        virtual void    visit(SyntaxNodeFunctionCall* node);
        virtual void    visit(SyntaxNodeArrayIndex* node);
        virtual void    visit(SyntaxNodePrimary* node);
        virtual void    visit(SyntaxNodeGrouping* node);

};

#endif
