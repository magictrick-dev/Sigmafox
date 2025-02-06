#ifndef SIGAMFOX_COMPILER_PARSER_BRANCHES_HPP
#define SIGAMFOX_COMPILER_PARSER_BRANCHES_HPP
#include <definitions.hpp>
#include <compiler/parser/visitor.hpp>
#include <compiler/parser/node.hpp>

// --- Root Syntax Node --------------------------------------------------------
//
// Acts as the trunk node for the entire syntax tree.
//

class SyntaxNodeRoot : public SyntaxNode
{
    public:
        inline                  SyntaxNodeRoot();
        inline virtual         ~SyntaxNodeRoot();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        vector<shared_ptr<SyntaxNode>> children;
};

SyntaxNodeRoot::
SyntaxNodeRoot()
{
    this->node_type = Nodetype::NODE_TYPE_ROOT;
}

SyntaxNodeRoot::
~SyntaxNodeRoot()
{

}

void SyntaxNodeRoot::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

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
        inline                  SyntaxNodeModule();
        inline virtual         ~SyntaxNodeModule();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        vector<shared_ptr<SyntaxNode>> children;
};

SyntaxNodeModule::
SyntaxNodeModule()
{
    this->node_type = Nodetype::NODE_TYPE_MODULE;
}

SyntaxNodeModule::
~SyntaxNodeModule()
{

}

void SyntaxNodeModule::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Main Syntax Node --------------------------------------------------------
//
// The main syntax node represents the main function of the program. The main
// function must be defined in the program, and it is the entry point for the
// program.
//

class SyntaxNodeMain : public SyntaxNode
{
    public:
        inline                  SyntaxNodeMain();
        inline virtual         ~SyntaxNodeMain();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        vector<shared_ptr<SyntaxNode>> children;
};

SyntaxNodeMain::
SyntaxNodeMain()
{
    this->node_type = Nodetype::NODE_TYPE_MAIN;
}

SyntaxNodeMain::
~SyntaxNodeMain()
{

}

void SyntaxNodeMain::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Include Statement Syntax Node -------------------------------------------
//
// Include statements are probably the busiest of all the syntax nodes since
// they perform several internal operations that validate the include paths
// and ensures that there are no circular dependencies.
//

class SyntaxNodeIncludeStatement : public SyntaxNode
{
    public:
        inline                  SyntaxNodeIncludeStatement();
        inline virtual         ~SyntaxNodeIncludeStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> module;
        string absolute_path;
        string relative_path;
        string user_path;
};

SyntaxNodeIncludeStatement::
SyntaxNodeIncludeStatement()
{
    this->node_type = Nodetype::NODE_TYPE_INCLUDE_STATEMENT;
}

SyntaxNodeIncludeStatement::
~SyntaxNodeIncludeStatement()
{

}

void SyntaxNodeIncludeStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Function Statement Syntax Node ------------------------------------------
//
// Function statements are occurrences of function definitions in the source code.
// These may occur either in the global scope or in the scope of other body scopes.
//

class SyntaxNodeFunctionStatement : public SyntaxNode
{

    public:
        inline                  SyntaxNodeFunctionStatement();
        inline virtual         ~SyntaxNodeFunctionStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        string signature;
        vector<string> parameters;
        vector<shared_ptr<SyntaxNode>> children;

};

SyntaxNodeFunctionStatement::
SyntaxNodeFunctionStatement()
{
    this->node_type = Nodetype::NODE_TYPE_FUNCTION_STATEMENT;
}

SyntaxNodeFunctionStatement::
~SyntaxNodeFunctionStatement()
{

}

void SyntaxNodeFunctionStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

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
        inline                  SyntaxNodeProcedureStatement();
        inline virtual         ~SyntaxNodeProcedureStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        string signature;
        vector<string> parameters;
        vector<shared_ptr<SyntaxNode>> children;

};

SyntaxNodeProcedureStatement::
SyntaxNodeProcedureStatement()
{
    this->node_type = Nodetype::NODE_TYPE_PROCEDURE_STATEMENT;
    this->signature = "void";
}

SyntaxNodeProcedureStatement::
~SyntaxNodeProcedureStatement()
{

}

void SyntaxNodeProcedureStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Expression Statement Syntax Node ----------------------------------------
//
// Expression statements are the most common type of statement in the language.
// They are used to evaluate expressions and perform operations on the data.
//

class SyntaxNodeExpressionStatement : public SyntaxNode
{

    public:
        inline                  SyntaxNodeExpressionStatement();
        inline virtual         ~SyntaxNodeExpressionStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> expression;

};

SyntaxNodeExpressionStatement::
SyntaxNodeExpressionStatement()
{
    this->node_type = Nodetype::NODE_TYPE_EXPRESSION_STATEMENT;
}

SyntaxNodeExpressionStatement::
~SyntaxNodeExpressionStatement()
{

}

void SyntaxNodeExpressionStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- While Statement Syntax Node ---------------------------------------------
//
// While statements are used to create loops in the program. They evaluate an
// expression and, if the expression is true, execute the body of the loop.
//

class SyntaxNodeWhileStatement : public SyntaxNode
{

    public:
        inline                  SyntaxNodeWhileStatement();
        inline virtual         ~SyntaxNodeWhileStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> expression;
        vector<shared_ptr<SyntaxNode>> children;

};

SyntaxNodeWhileStatement::
SyntaxNodeWhileStatement()
{
    this->node_type = Nodetype::NODE_TYPE_WHILE_STATEMENT;
}

SyntaxNodeWhileStatement::
~SyntaxNodeWhileStatement()
{

}

void SyntaxNodeWhileStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Loop Statement Syntax Node ----------------------------------------------
//
// Loop statements are used to create basic for-loops pretty typical of most
// languages. The original COSY specification ensured that the iterator remains
// constant through the loop, however we will not enforce this requirement.
// 

class SyntaxNodeLoopStatement : public SyntaxNode
{

    public:
        inline                  SyntaxNodeLoopStatement();
        inline virtual         ~SyntaxNodeLoopStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string iterator;
        shared_ptr<SyntaxNode> start;
        shared_ptr<SyntaxNode> end;
        shared_ptr<SyntaxNode> step;
        vector<shared_ptr<SyntaxNode>> children;

};

SyntaxNodeLoopStatement::
SyntaxNodeLoopStatement()
{
    this->node_type = Nodetype::NODE_TYPE_LOOP_STATEMENT;
}

SyntaxNodeLoopStatement::
~SyntaxNodeLoopStatement()
{

}

void SyntaxNodeLoopStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Variable Statement Syntax Node -----------------------------------------
//
// Variable statements are used to declare variables in the program. They are
// used to allocate memory for the variable and assign an initial value. Variables
// may contain dimensions, indicating that they are arrays. They may also contain
// a storage constant, however this is a product of the legacy implementation of
// COSY and isn't a requirement for this language.
//
// Additionally, COSY doesn't allow for inline variable definitions, but we can
// support this trivially by allowing the expression to be optional.
//

class SyntaxNodeVariableStatement : public SyntaxNode
{

    public:
        inline                  SyntaxNodeVariableStatement();
        inline virtual         ~SyntaxNodeVariableStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        string signature;
        shared_ptr<SyntaxNode> storage;
        shared_ptr<SyntaxNode> expression;
        vector<shared_ptr<SyntaxNode>> dimensions;

};

SyntaxNodeVariableStatement::
SyntaxNodeVariableStatement()
{
    this->node_type = Nodetype::NODE_TYPE_VARIABLE_STATEMENT;
    this->signature = "int64_t";
}

SyntaxNodeVariableStatement::
~SyntaxNodeVariableStatement()
{

}

void SyntaxNodeVariableStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

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
        inline                  SyntaxNodeScopeStatement();
        inline virtual         ~SyntaxNodeScopeStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        vector<shared_ptr<SyntaxNode>> children;

};

SyntaxNodeScopeStatement::
SyntaxNodeScopeStatement()
{
    this->node_type = Nodetype::NODE_TYPE_SCOPE_STATEMENT;
}

SyntaxNodeScopeStatement::
~SyntaxNodeScopeStatement()
{

}

void SyntaxNodeScopeStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

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
        inline                  SyntaxNodeConditionalStatement();
        inline virtual         ~SyntaxNodeConditionalStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> expression;
        shared_ptr<SyntaxNode> next;
        vector<shared_ptr<SyntaxNode>> children;

};

SyntaxNodeConditionalStatement::
SyntaxNodeConditionalStatement()
{
    this->node_type = Nodetype::NODE_TYPE_CONDITIONAL_STATEMENT;
}

SyntaxNodeConditionalStatement::
~SyntaxNodeConditionalStatement()
{

}

void SyntaxNodeConditionalStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Read Statement Syntax Node ---------------------------------------------
//
// Read statements are used to read input from the user. They are used to read
// input from the console and store it in a variable.
//

class SyntaxNodeReadStatement : public SyntaxNode
{

    public:
        inline                  SyntaxNodeReadStatement();
        inline virtual         ~SyntaxNodeReadStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        shared_ptr<SyntaxNode> location;
        shared_ptr<SyntaxNode> expression;

};

SyntaxNodeReadStatement::
SyntaxNodeReadStatement()
{
    this->node_type = Nodetype::NODE_TYPE_READ_STATEMENT;
}

SyntaxNodeReadStatement::
~SyntaxNodeReadStatement()
{

}

void SyntaxNodeReadStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Write Statement Syntax Node --------------------------------------------
//
// Write statements are used to write output to the console. They are used to
// write output to the console and display it to the user.
//

class SyntaxNodeWriteStatement : public SyntaxNode
{

    public:
        inline                  SyntaxNodeWriteStatement();
        inline virtual         ~SyntaxNodeWriteStatement();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> location;
        vector<shared_ptr<SyntaxNode>> expressions;

};

SyntaxNodeWriteStatement::
SyntaxNodeWriteStatement()
{
    this->node_type = Nodetype::NODE_TYPE_WRITE_STATEMENT;
}

SyntaxNodeWriteStatement::
~SyntaxNodeWriteStatement()
{

}

void SyntaxNodeWriteStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

#endif