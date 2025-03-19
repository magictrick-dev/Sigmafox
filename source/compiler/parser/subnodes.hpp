#ifndef SIGMAFOX_COMPILER_PARSER_SUBNODES_HPP
#define SIGMAFOX_COMPILER_PARSER_SUBNODES_HPP
#include <definitions.hpp>
#include <compiler/parser/node.hpp>

// --- Root Syntax Node --------------------------------------------------------
//
// Acts as the trunk node for the entire syntax tree.
//

class SyntaxNodeRoot : public SyntaxNode
{
    public:
                         SyntaxNodeRoot();
        virtual         ~SyntaxNodeRoot();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        vector<SyntaxNode*> children;
};

// --- Module Syntax Node ------------------------------------------------------
//
// Represents a module in the syntax tree. Essentially, an include statement
// generates an include node which, if this is the first time it is encountered,
// will generate a module node. Only one module node is allowed per unique source
// lest we encounter a redefinition error.
//

class SyntaxNodeModule : public SyntaxNode
{
    public:
                         SyntaxNodeModule();
        virtual         ~SyntaxNodeModule();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        vector<SyntaxNode*> children;
};

// --- Main Syntax Node --------------------------------------------------------
//
// The main syntax node represents the main function of the program. The main
// function must be defined in the program, and it is the entry point for the
// program.
//

class SyntaxNodeMain : public SyntaxNode
{
    public:
                         SyntaxNodeMain();
        virtual         ~SyntaxNodeMain();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        vector<SyntaxNode*> children;
};

// --- Include Statement Syntax Node -------------------------------------------
//
// Include statements are probably the busiest of all the syntax nodes since
// they perform several internal operations that validate the include paths
// and ensures that there are no circular dependencies.
//

class SyntaxNodeIncludeStatement : public SyntaxNode
{
    public:
                         SyntaxNodeIncludeStatement();
        virtual         ~SyntaxNodeIncludeStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        SyntaxNode* module;
        string absolute_path;
        string relative_path;
        string user_path;
};

// --- Variable Statement Syntax Node -----------------------------------------
//
// Variable statements are used to declare variables in the program. They are
// used to allocate memory for the variable and assign an initial value. Variables
// may contain dimensions, indicating that they are arrays. They may also contain
// a storage constant, however this is a product of the legacy implementation of
// COSY and isn't a requirement for this language.
//
// Additionally, COSY doesn't allow for variable definitions, but we can
// support this trivially by allowing the expression to be optional.
//

class SyntaxNodeVariableStatement : public SyntaxNode
{

    public:
                         SyntaxNodeVariableStatement();
        virtual         ~SyntaxNodeVariableStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        SyntaxNode* storage;
        SyntaxNode* expression;
        vector<SyntaxNode*> dimensions;

        Datatype data_type;
        Structuretype structure_type;

};

// --- Function Statement Syntax Node ------------------------------------------
//
// Function statements are occurrences of function definitions in the source code.
// These may occur either in the global scope or in the scope of other body scopes.
//

class SyntaxNodeFunctionStatement : public SyntaxNode
{

    public:
                         SyntaxNodeFunctionStatement();
        virtual         ~SyntaxNodeFunctionStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        bool is_global;
        SyntaxNodeVariableStatement *variable_node;
        vector<SyntaxNodeVariableStatement*> parameters;
        vector<SyntaxNode*> children;

};

// --- Procedure Statement Syntax Node -----------------------------------------
//
// Procedure statements are occurrences of procedure definitions in the source code.
// These may occur either in the global scope or in the scope of other body scopes.
// Unlike functions, procedures do not return anything and are used entirely for
// side-effects.
//

class SyntaxNodeProcedureStatement : public SyntaxNode
{

    public:
                         SyntaxNodeProcedureStatement();
        virtual         ~SyntaxNodeProcedureStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        bool is_global;
        SyntaxNodeVariableStatement *variable_node;
        vector<SyntaxNodeVariableStatement*> parameters;
        vector<SyntaxNode*> children;

};

// --- Expression Statement Syntax Node ----------------------------------------
//
// Expression statements are the most common type of statement in the language.
// They are used to evaluate expressions and perform operations on the data.
//

class SyntaxNodeExpressionStatement : public SyntaxNode
{

    public:
                         SyntaxNodeExpressionStatement();
        virtual         ~SyntaxNodeExpressionStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        SyntaxNode* expression;

};

// --- While Statement Syntax Node ---------------------------------------------
//
// While statements are used to create loops in the program. They evaluate an
// expression and, if the expression is true, execute the body of the loop.
//

class SyntaxNodeWhileStatement : public SyntaxNode
{

    public:
                         SyntaxNodeWhileStatement();
        virtual         ~SyntaxNodeWhileStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        SyntaxNode* expression;
        vector<SyntaxNode*> children;

};

// --- Loop Statement Syntax Node ----------------------------------------------
//
// Loop statements are used to create basic for-loops pretty typical of most
// languages. The original COSY specification ensured that the iterator remains
// constant through the loop, however we will not enforce this requirement.
// 

class SyntaxNodeLoopStatement : public SyntaxNode
{

    public:
                         SyntaxNodeLoopStatement();
        virtual         ~SyntaxNodeLoopStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string iterator;

        SyntaxNode* variable;
        SyntaxNode* start;
        SyntaxNode* end;
        SyntaxNode* step;
        vector<SyntaxNode*> children;

};

// --- Scope Statement Syntax Node --------------------------------------------
//
// Scope statements are used to create new scopes in the program. They are used
// to create new blocks of code that can be executed in isolation from the rest
// of the program. This is useful for creating new variable scopes and for creating
// new function and procedure definitions.
//
// This is not in the original COSY specification, but it is a common enough feature
// that it would prove valuable to include.
//

class SyntaxNodeScopeStatement : public SyntaxNode
{

    public:
                         SyntaxNodeScopeStatement();
        virtual         ~SyntaxNodeScopeStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        vector<SyntaxNode*> children;

};

// --- Conditional Statement Syntax Node --------------------------------------
//
// Conditional statements are used to create branches in the program. They evaluate
// an expression and, if the expression is true, execute the body of the conditional
// statement. COSY doesn't have a concept of "else" statements, instead it uses
// "elseif" with a truthy expression to create a new branch.
//

class SyntaxNodeConditionalStatement : public SyntaxNode
{

    public:
                         SyntaxNodeConditionalStatement();
        virtual         ~SyntaxNodeConditionalStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        SyntaxNode* expression;
        SyntaxNodeConditionalStatement* next;
        vector<SyntaxNode*> children;

};

// --- Read Statement Syntax Node ---------------------------------------------
//
// Read statements are used to read input from the user. They are used to read
// input from the console and store it in a variable.
//

class SyntaxNodeReadStatement : public SyntaxNode
{

    public:
                         SyntaxNodeReadStatement();
        virtual         ~SyntaxNodeReadStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        SyntaxNode* location;

};

// --- Write Statement Syntax Node --------------------------------------------
//
// Write statements are used to write output to the console. They are used to
// write output to the console and display it to the user.
//

class SyntaxNodeWriteStatement : public SyntaxNode
{

    public:
                         SyntaxNodeWriteStatement();
        virtual         ~SyntaxNodeWriteStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        SyntaxNode* location;
        vector<SyntaxNode*> expressions;

};


// --- Expression Syntax Node ---------------------------------------------------
//
// Expression nodes are used to represent expressions in the syntax tree. They
// are used to evaluate expressions and perform operations on the data.
//

class SyntaxNodeExpression : public SyntaxNode
{
    public:
                         SyntaxNodeExpression();
        virtual         ~SyntaxNodeExpression();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        SyntaxNode* expression;

};

// --- Procedure Call Syntax Node ----------------------------------------------
//
// For procedure call invocations.
//

class SyntaxNodeProcedureCall : public SyntaxNode
{

    public:
                         SyntaxNodeProcedureCall();
        virtual         ~SyntaxNodeProcedureCall();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        vector<SyntaxNode*> arguments;

};

// --- Assignment Syntax Node ---------------------------------------------------
//
// Assignment nodes are used to represent assignment operations in the syntax tree.
// They are used to assign values to variables and other data structures.
//

class SyntaxNodeAssignment : public SyntaxNode
{

    public:
                         SyntaxNodeAssignment();
        virtual         ~SyntaxNodeAssignment();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        SyntaxNode* left;
        SyntaxNode* right;

};

// --- Equality Syntax Node -----------------------------------------------------
//
// Equality nodes are used to represent equality operations in the syntax tree.
// They are used to compare two values for equality.
//

class SyntaxNodeEquality : public SyntaxNode
{

    public:
                         SyntaxNodeEquality();
        virtual         ~SyntaxNodeEquality();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        SyntaxNode* left;
        SyntaxNode* right;

};

// --- Comparison Syntax Node ---------------------------------------------------
//
// Comparison nodes are used to represent comparison operations in the syntax tree.
// They are used to compare two values for inequality.
//

class SyntaxNodeComparison : public SyntaxNode
{

    public:
                         SyntaxNodeComparison();
        virtual         ~SyntaxNodeComparison();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        SyntaxNode* left;
        SyntaxNode* right;

};

// --- Term Syntax Node ---------------------------------------------------------
//
// Term nodes are used to represent term operations in the syntax tree. They are
// used to evaluate terms and perform operations on the data.
//

class SyntaxNodeTerm : public SyntaxNode
{

    public:
                         SyntaxNodeTerm();
        virtual         ~SyntaxNodeTerm();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        SyntaxNode* left;
        SyntaxNode* right;

};

// --- Factor Syntax Node -------------------------------------------------------
//
// Factor nodes are used to represent factor operations in the syntax tree. They
// are used to evaluate factors and perform operations on the data.
//

class SyntaxNodeFactor : public SyntaxNode
{

    public:
                         SyntaxNodeFactor();
        virtual         ~SyntaxNodeFactor();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        SyntaxNode* left;
        SyntaxNode* right;

};

// --- Magnitude Syntax Node ----------------------------------------------------
//
// Magnitude nodes are used to represent magnitude operations in the syntax tree.
// They are used to evaluate magnitudes and perform operations on the data.
//

class SyntaxNodeMagnitude : public SyntaxNode
{

    public:
                         SyntaxNodeMagnitude();
        virtual         ~SyntaxNodeMagnitude();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        SyntaxNode* left;
        SyntaxNode* right;

};

// --- Extraction Syntax Node ---------------------------------------------------
//
// Extraction nodes are used to represent extraction operations in the syntax tree.
// They are used to extract values from data structures.
//

class SyntaxNodeExtraction : public SyntaxNode
{

    public:
                         SyntaxNodeExtraction();
        virtual         ~SyntaxNodeExtraction();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        SyntaxNode* left;
        SyntaxNode* right;

};

// --- Derivation Syntax Node ---------------------------------------------------
//
// Derivation nodes are used to represent derivation operations in the syntax tree.
// They are used to derive values from data structures.
//

class SyntaxNodeDerivation : public SyntaxNode
{

    public:
                         SyntaxNodeDerivation();
        virtual         ~SyntaxNodeDerivation();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        SyntaxNode* left;
        SyntaxNode* right;

};

// --- Unary Syntax Node --------------------------------------------------------
//
// Unary nodes are used to represent unary operations in the syntax tree. They are
// used to evaluate unary operations and perform operations on the data.
//

class SyntaxNodeUnary : public SyntaxNode
{

    public:
                         SyntaxNodeUnary();
        virtual         ~SyntaxNodeUnary();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        SyntaxNode* expression;

};

// --- Function Call Syntax Node ------------------------------------------------
//
// Function call nodes are used to represent function calls in the syntax tree.
// They are used to call functions and pass arguments to them.
//

class SyntaxNodeFunctionCall : public SyntaxNode
{

    public:
                         SyntaxNodeFunctionCall();
        virtual         ~SyntaxNodeFunctionCall();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        vector<SyntaxNode*> arguments;

};

// --- Array Index Syntax Node --------------------------------------------------
//
// Array index nodes are used to represent array index operations in the syntax tree.
// They are used to access elements of an array.
//

class SyntaxNodeArrayIndex : public SyntaxNode
{

    public:
                         SyntaxNodeArrayIndex();
        virtual         ~SyntaxNodeArrayIndex();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        vector<SyntaxNode*> indices;

};

// --- Primary Syntax Node ------------------------------------------------------
//
// Primary nodes are used to represent primary operations in the syntax tree. They
// are used to evaluate primary operations and perform operations on the data.
//

class SyntaxNodePrimary : public SyntaxNode
{

    public:
                         SyntaxNodePrimary();
        virtual         ~SyntaxNodePrimary();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Primarytype primarytype;
        string primitive;

};

// --- Grouping Syntax Node -----------------------------------------------------
//
// Grouping nodes are used to represent grouping operations in the syntax tree.
// They are used to group expressions together.
//

class SyntaxNodeGrouping : public SyntaxNode
{

    public:
                         SyntaxNodeGrouping();
        virtual         ~SyntaxNodeGrouping();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        SyntaxNode* expression;

};


#endif
