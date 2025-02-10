#include <compiler/parser/statements.hpp>
#include <compiler/parser/visitor.hpp>

// --- Root Syntax Node --------------------------------------------------------

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

// --- Parameter Syntax Node ---------------------------------------------------

SyntaxNodeParameter::
SyntaxNodeParameter()
{
    this->node_type = Nodetype::NODE_TYPE_PARAMETER;
}

SyntaxNodeParameter::
~SyntaxNodeParameter()
{

}

void SyntaxNodeParameter::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Function Statement Syntax Node ------------------------------------------

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

// --- Procedure Call Statement Syntax Node ------------------------------------

SyntaxNodeProcedureCallStatement::
SyntaxNodeProcedureCallStatement()
{
    this->node_type = Nodetype::NODE_TYPE_PROCEDURE_CALL_STATEMENT;
}

SyntaxNodeProcedureCallStatement::
~SyntaxNodeProcedureCallStatement()
{

}

void SyntaxNodeProcedureCallStatement::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- While Statement Syntax Node ---------------------------------------------

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

