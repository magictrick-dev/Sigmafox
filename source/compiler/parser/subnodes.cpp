#include <compiler/parser/subnodes.hpp>
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
    this->data_type = Datatype::DATA_TYPE_UNKNOWN;
    this->structure_type = Structuretype::STRUCTURE_TYPE_UNKNOWN;
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

// --- Expression Syntax Node ---------------------------------------------------

SyntaxNodeExpression::
SyntaxNodeExpression()
{
    this->node_type = Nodetype::NODE_TYPE_EXPRESSION;
}

SyntaxNodeExpression::
~SyntaxNodeExpression()
{

}

void SyntaxNodeExpression::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Procedure Call Statement Syntax Node ------------------------------------

SyntaxNodeProcedureCall::
SyntaxNodeProcedureCall()
{
    this->node_type = Nodetype::NODE_TYPE_PROCEDURE_CALL;
}

SyntaxNodeProcedureCall::
~SyntaxNodeProcedureCall()
{

}

void SyntaxNodeProcedureCall::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Assignment Syntax Node ---------------------------------------------------

SyntaxNodeAssignment::
SyntaxNodeAssignment()
{
    this->node_type = Nodetype::NODE_TYPE_ASSIGNMENT;
}

SyntaxNodeAssignment::
~SyntaxNodeAssignment()
{

}

void SyntaxNodeAssignment::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Equality Syntax Node -----------------------------------------------------

SyntaxNodeEquality::
SyntaxNodeEquality()
{
    this->node_type = Nodetype::NODE_TYPE_EQUALITY;
    this->operation = Operationtype::OPERATION_TYPE_UNKNOWN;
}

SyntaxNodeEquality::
~SyntaxNodeEquality()
{

}

void SyntaxNodeEquality::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Comparison Syntax Node ---------------------------------------------------

SyntaxNodeComparison::
SyntaxNodeComparison()
{
    this->node_type = Nodetype::NODE_TYPE_COMPARISON;
    this->operation = Operationtype::OPERATION_TYPE_UNKNOWN;
}

SyntaxNodeComparison::
~SyntaxNodeComparison()
{

}

void SyntaxNodeComparison::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Term Syntax Node ---------------------------------------------------------

SyntaxNodeTerm::
SyntaxNodeTerm()
{
    this->node_type = Nodetype::NODE_TYPE_TERM;
    this->operation = Operationtype::OPERATION_TYPE_UNKNOWN;
}

SyntaxNodeTerm::
~SyntaxNodeTerm()
{

}

void SyntaxNodeTerm::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Factor Syntax Node -------------------------------------------------------

SyntaxNodeFactor::
SyntaxNodeFactor()
{
    this->node_type = Nodetype::NODE_TYPE_FACTOR;
    this->operation = Operationtype::OPERATION_TYPE_UNKNOWN;
}

SyntaxNodeFactor::
~SyntaxNodeFactor()
{

}

void SyntaxNodeFactor::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Magnitude Syntax Node ----------------------------------------------------

SyntaxNodeMagnitude::
SyntaxNodeMagnitude()
{
    this->node_type = Nodetype::NODE_TYPE_MAGNITUDE;
    this->operation = Operationtype::OPERATION_TYPE_UNKNOWN;
}

SyntaxNodeMagnitude::
~SyntaxNodeMagnitude()
{

}

void SyntaxNodeMagnitude::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Extraction Syntax Node ---------------------------------------------------

SyntaxNodeExtraction::
SyntaxNodeExtraction()
{
    this->node_type = Nodetype::NODE_TYPE_EXTRACTION;
}

SyntaxNodeExtraction::
~SyntaxNodeExtraction()
{

}

void SyntaxNodeExtraction::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Derivation Syntax Node ---------------------------------------------------

SyntaxNodeDerivation::
SyntaxNodeDerivation()
{
    this->node_type = Nodetype::NODE_TYPE_DERIVATION;
    this->operation = Operationtype::OPERATION_TYPE_UNKNOWN;
}

SyntaxNodeDerivation::
~SyntaxNodeDerivation()
{

}

void SyntaxNodeDerivation::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Unary Syntax Node --------------------------------------------------------

SyntaxNodeUnary::
SyntaxNodeUnary()
{
    this->node_type = Nodetype::NODE_TYPE_UNARY;
    this->operation = Operationtype::OPERATION_TYPE_UNKNOWN;
}

SyntaxNodeUnary::
~SyntaxNodeUnary()
{

}

void SyntaxNodeUnary::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Function Call Syntax Node ------------------------------------------------

SyntaxNodeFunctionCall::
SyntaxNodeFunctionCall()
{
    this->node_type = Nodetype::NODE_TYPE_FUNCTION_CALL;
}

SyntaxNodeFunctionCall::
~SyntaxNodeFunctionCall()
{

}

void SyntaxNodeFunctionCall::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Array Index Syntax Node --------------------------------------------------

SyntaxNodeArrayIndex::
SyntaxNodeArrayIndex()
{
    this->node_type = Nodetype::NODE_TYPE_ARRAY_INDEX;
}

SyntaxNodeArrayIndex::
~SyntaxNodeArrayIndex()
{

}

void SyntaxNodeArrayIndex::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Primary Syntax Node ------------------------------------------------------

SyntaxNodePrimary::
SyntaxNodePrimary()
{
    this->node_type = Nodetype::NODE_TYPE_PRIMARY;
}

SyntaxNodePrimary::
~SyntaxNodePrimary()
{

}

void SyntaxNodePrimary::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}

// --- Grouping Syntax Node -----------------------------------------------------

SyntaxNodeGrouping::
SyntaxNodeGrouping()
{
    this->node_type = Nodetype::NODE_TYPE_GROUPING;
}

SyntaxNodeGrouping::
~SyntaxNodeGrouping()
{

}

void SyntaxNodeGrouping::
accept(SyntaxNodeVisitor *visitor)
{
    visitor->visit(this);
}


