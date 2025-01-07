#ifndef SIGMAFOX_COMPILER_SYNTAX_NODE_HPP
#define SIGMAFOX_COMPILER_SYNTAX_NODE_HPP
#include <vector>
#include <string>
#include <definitions.hpp>
#include <compiler/tokenizer.hpp>

class SyntaxNodeRoot;
class SyntaxNodeModule;

class SyntaxNodeInclude;

class SyntaxNodeMain;
class SyntaxNodeExpressionStatement;
class SyntaxNodeFunctionStatement;
class SyntaxNodeVariableStatement;
class SyntaxNodeScopeStatement;
class SyntaxNodeProcedureStatement;

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
        virtual void visit_SyntaxNodeRoot(SyntaxNodeRoot *node)     = 0;
        virtual void visit_SyntaxNodeModule(SyntaxNodeModule *node) = 0;
        virtual void visit_SyntaxNodeMain(SyntaxNodeMain *node)     = 0;

        virtual void visit_SyntaxNodeInclude(SyntaxNodeInclude *node) = 0;
        virtual void visit_SyntaxNodeExpressionStatement(SyntaxNodeExpressionStatement *node) = 0;
        virtual void visit_SyntaxNodeVariableStatement(SyntaxNodeVariableStatement *node) = 0;
        virtual void visit_SyntaxNodeScopeStatement(SyntaxNodeScopeStatement *node) = 0;
        virtual void visit_SyntaxNodeFunctionStatement(SyntaxNodeFunctionStatement *node) = 0;
        virtual void visit_SyntaxNodeProcedureStatement(SyntaxNodeProcedureStatement *node) = 0;

        virtual void visit_SyntaxNodeExpression(SyntaxNodeExpression *node)         = 0;
        virtual void visit_SyntaxNodeProcedureCall(SyntaxNodeProcedureCall *node)   = 0;
        virtual void visit_SyntaxNodeAssignment(SyntaxNodeAssignment *node)         = 0;
        virtual void visit_SyntaxNodeEquality(SyntaxNodeEquality *node)             = 0;
        virtual void visit_SyntaxNodeComparison(SyntaxNodeComparison *node)         = 0;
        virtual void visit_SyntaxNodeTerm(SyntaxNodeTerm *node)                     = 0;
        virtual void visit_SyntaxNodeFactor(SyntaxNodeFactor *node)                 = 0;
        virtual void visit_SyntaxNodeMagnitude(SyntaxNodeMagnitude *node)           = 0;
        virtual void visit_SyntaxNodeExtraction(SyntaxNodeExtraction *node)         = 0;
        virtual void visit_SyntaxNodeDerivation(SyntaxNodeDerivation *node)         = 0;
        virtual void visit_SyntaxNodeUnary(SyntaxNodeUnary *node)                   = 0;
        virtual void visit_SyntaxNodeFunctionCall(SyntaxNodeFunctionCall *node)     = 0;
        virtual void visit_SyntaxNodeArrayIndex(SyntaxNodeArrayIndex *node)         = 0;
        virtual void visit_SyntaxNodePrimary(SyntaxNodePrimary *node)               = 0;
        virtual void visit_SyntaxNodeGrouping(SyntaxNodeGrouping *node)             = 0;

};

// Provides a way of easily identifying which node was encountered.
enum class SyntaxNodeType
{
    NodeTypeVoid,
    NodeTypeRoot,
    NodeTypeModule,

    NodeTypeMain,

    NodeTypeInclude,
    NodeTypeExpressionStatement,
    NodeTypeVariableStatement,
    NodeTypeScopeStatement,
    NodeTypeFunctionStatement,
    NodeTypeProcedureStatement,
    
    NodeTypeExpression,
    NodeTypeProcedureCall,
    NodeTypeAssignment,
    NodeTypeEquality,
    NodeTypeComparison,
    NodeTypeTerm,
    NodeTypeFactor,
    NodeTypeMagnitude,
    NodeTypeExtraction,
    NodeTypeDerivation,
    NodeTypeUnary,
    NodeTypeFunctionCall,
    NodeTypeArrayIndex,
    NodeTypePrimary,
    NodeTypeGrouping,
};

// --- Abstract Syntax Node Base Class -----------------------------------------
//
// The abstract node basically provides the universal feature set of all AST
// nodes. There's a way to inspect the type of node (all nodes are defined with
// a given type and are always reliably that type) and a way to cast the syntax
// node to its given sub-class. The current implementation is a dynamic_cast wrapper,
// but if we want to do anything fancy in the future, internal functionality could
// change to something more complex.
//
// The "accept" visitor method is an important function to override, as it allows
// visitor interfaces to propagate to their respective node types without having
// to actually manually inspect the type, cast, and perform the operation. Of
// course, this comes with a performance penalty, so we don't want to do this
// for something more intense.
//
// The "is_void" function determines if the type of the base node is void. This
// basically shorthands the type check for when a void node is returned. Use this
// over "cast_to" or "get_type" when checking for void nodes.
//

class ISyntaxNode 
{
    public:
        inline          ISyntaxNode() { this->type = SyntaxNodeType::NodeTypeVoid; }
        inline virtual ~ISyntaxNode() { }

        inline SyntaxNodeType       get_type() const { return this->type; }
        template <class T> inline T cast_to() { return dynamic_cast<T>(this); }

        virtual void accept(ISyntaxNodeVisitor *visitor) = 0;

    protected:
        SyntaxNodeType type;

};

#endif
