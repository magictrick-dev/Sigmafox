#ifndef SIGMAFOX_COMPILER_SYNTAX_NODE_HPP
#define SIGMAFOX_COMPILER_SYNTAX_NODE_HPP
#include <vector>
#include <string>
#include <definitions.hpp>
#include <compiler/tokenizer.hpp>
#include <compiler/syntaxvisitor.hpp>

// Provides a way of easily identifying which node was encountered.
enum class SyntaxNodeType
{
    NodeTypeVoid,
    NodeTypeRoot,
    NodeTypeModule,

    NodeTypeMain,

    NodeTypeInclude,
    NodeTypeExpressionStatement,
    NodeTypeWhileStatement,
    NodeTypeLoopStatement,
    NodeTypeVariableStatement,
    NodeTypeScopeStatement,
    NodeTypeFunctionStatement,
    NodeTypeProcedureStatement,
    NodeTypeIfStatement,
    NodeTypeConditional,
    NodeTypeReadStatement,
    NodeTypeWriteStatement,
    
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
