#ifndef SIGAMFOX_COMPILER_PARSER_BRANCHES_HPP
#define SIGAMFOX_COMPILER_PARSER_BRANCHES_HPP
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
        vector<shared_ptr<SyntaxNode>> children;
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
        vector<shared_ptr<SyntaxNode>> children;
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
        vector<shared_ptr<SyntaxNode>> children;
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
        shared_ptr<SyntaxNode> module;
        string absolute_path;
        string relative_path;
        string user_path;
};

// --- Parameter Syntax Node ---------------------------------------------------
//
// Represents a parameter in a function or procedure definition.
//

class SyntaxNodeParameter : public SyntaxNode
{
    public:
                         SyntaxNodeParameter();
        virtual         ~SyntaxNodeParameter();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;

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
        string identifier;
        vector<shared_ptr<SyntaxNode>> parameters;
        vector<shared_ptr<SyntaxNode>> children;

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
        string identifier;
        vector<shared_ptr<SyntaxNode>> parameters;
        vector<shared_ptr<SyntaxNode>> children;

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
        shared_ptr<SyntaxNode> expression;

};

// --- Procedure Call Statement Syntax Node ------------------------------------
//
// For procedure call invocations.
//

class SyntaxNodeProcedureCallStatement : public SyntaxNode
{

    public:
                         SyntaxNodeProcedureCallStatement();
        virtual         ~SyntaxNodeProcedureCallStatement();
        virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        vector<shared_ptr<SyntaxNode>> arguments;

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
        shared_ptr<SyntaxNode> expression;
        vector<shared_ptr<SyntaxNode>> children;

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
        shared_ptr<SyntaxNode> start;
        shared_ptr<SyntaxNode> end;
        shared_ptr<SyntaxNode> step;
        vector<shared_ptr<SyntaxNode>> children;

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
        shared_ptr<SyntaxNode> storage;
        shared_ptr<SyntaxNode> expression;
        vector<shared_ptr<SyntaxNode>> dimensions;

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
        vector<shared_ptr<SyntaxNode>> children;

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
        shared_ptr<SyntaxNode> expression;
        shared_ptr<SyntaxNode> next;
        vector<shared_ptr<SyntaxNode>> children;

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
        shared_ptr<SyntaxNode> location;
        shared_ptr<SyntaxNode> expression;

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
        shared_ptr<SyntaxNode> location;
        vector<shared_ptr<SyntaxNode>> expressions;

};

#endif