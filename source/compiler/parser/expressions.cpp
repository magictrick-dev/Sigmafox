#include <compiler/parser/expressions.hpp>
#include <compiler/parser/visitor.hpp>

// --- Expression Syntax Node ---------------------------------------------------

SyntaxNodeExpression::
SyntaxNodeExpression()
{
    this->nodetype = Nodetype::NODE_TYPE_EXPRESSION;
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

// --- Assignment Syntax Node ---------------------------------------------------

SyntaxNodeAssignment::
SyntaxNodeAssignment()
{
    this->nodetype = Nodetype::NODE_TYPE_ASSIGNMENT;
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
    this->nodetype = Nodetype::NODE_TYPE_EQUALITY;
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
    this->nodetype = Nodetype::NODE_TYPE_COMPARISON;
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
    this->nodetype = Nodetype::NODE_TYPE_TERM;
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
    this->nodetype = Nodetype::NODE_TYPE_FACTOR;
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
    this->nodetype = Nodetype::NODE_TYPE_MAGNITUDE;
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
    this->nodetype = Nodetype::NODE_TYPE_EXTRACTION;
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
    this->nodetype = Nodetype::NODE_TYPE_DERIVATION;
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
    this->nodetype = Nodetype::NODE_TYPE_UNARY;
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
    this->nodetype = Nodetype::NODE_TYPE_FUNCTION_CALL;
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
    this->nodetype = Nodetype::NODE_TYPE_ARRAY_INDEX;
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
    this->nodetype = Nodetype::NODE_TYPE_PRIMARY;
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
    this->nodetype = Nodetype::NODE_TYPE_GROUPING;
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
