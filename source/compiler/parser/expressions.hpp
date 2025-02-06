#ifndef SIGMAFOX_COMPILER_PARSER_EXPRESSIONS_HPP
#define SIGMAFOX_COMPILER_PARSER_EXPRESSIONS_HPP
#include <definitions.hpp>
#include <compiler/parser/visitor.hpp>
#include <compiler/parser/node.hpp>

enum class Operationtype
{
    OPERATION_TYPE_UNKNOWN,

    OPERATION_TYPE_ADDITION,
    OPERATION_TYPE_SUBTRACTION,
    OPERATION_TYPE_MULTIPLICATION,
    OPERATION_TYPE_DIVISION,
    OPERATION_TYPE_POWER,
    OPERATION_TYPE_ROOT,
    OPERATION_TYPE_EXTRACTION,
    OPERATION_TYPE_DERIVATION,

    OPERATION_TYPE_EQUALS,
    OPERATION_TYPE_NOT_EQUALS,
    OPERATION_TYPE_LESS_THAN,
    OPERATION_TYPE_LESS_THAN_OR_EQUAL,
    OPERATION_TYPE_GREATER_THAN,
    OPERATION_TYPE_GREATER_THAN_OR_EQUAL,
};

enum class Primarytype
{
    PRIMARY_TYPE_UNKNOWN,

    PRIMARY_TYPE_INTEGER,
    PRIMARY_TYPE_REAL,
    PRIMARY_TYPE_COMPLEX,
    PRIMARY_TYPE_STRING,
    PRIMARY_TYPE_IDENTIFIER,
};

// --- Expression Syntax Node ---------------------------------------------------
//
// Expression nodes are used to represent expressions in the syntax tree. They
// are used to evaluate expressions and perform operations on the data.
//

class SyntaxNodeExpression : public SyntaxNode
{
    public:
        inline                  SyntaxNodeExpression();
        inline virtual         ~SyntaxNodeExpression();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> expression;

};

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

// --- Assignment Syntax Node ---------------------------------------------------
//
// Assignment nodes are used to represent assignment operations in the syntax tree.
// They are used to assign values to variables and other data structures.
//

class SyntaxNodeAssignment : public SyntaxNode
{

    public:
        inline                  SyntaxNodeAssignment();
        inline virtual         ~SyntaxNodeAssignment();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

};

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
//
// Equality nodes are used to represent equality operations in the syntax tree.
// They are used to compare two values for equality.
//

class SyntaxNodeEquality : public SyntaxNode
{

    public:
        inline                  SyntaxNodeEquality();
        inline virtual         ~SyntaxNodeEquality();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

};

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
//
// Comparison nodes are used to represent comparison operations in the syntax tree.
// They are used to compare two values for inequality.
//

class SyntaxNodeComparison : public SyntaxNode
{

    public:
        inline                  SyntaxNodeComparison();
        inline virtual         ~SyntaxNodeComparison();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

};

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
//
// Term nodes are used to represent term operations in the syntax tree. They are
// used to evaluate terms and perform operations on the data.
//

class SyntaxNodeTerm : public SyntaxNode
{

    public:
        inline                  SyntaxNodeTerm();
        inline virtual         ~SyntaxNodeTerm();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

};

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
//
// Factor nodes are used to represent factor operations in the syntax tree. They
// are used to evaluate factors and perform operations on the data.
//

class SyntaxNodeFactor : public SyntaxNode
{

    public:
        inline                  SyntaxNodeFactor();
        inline virtual         ~SyntaxNodeFactor();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

};

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
//
// Magnitude nodes are used to represent magnitude operations in the syntax tree.
// They are used to evaluate magnitudes and perform operations on the data.
//

class SyntaxNodeMagnitude : public SyntaxNode
{

    public:
        inline                  SyntaxNodeMagnitude();
        inline virtual         ~SyntaxNodeMagnitude();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

};

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
//
// Extraction nodes are used to represent extraction operations in the syntax tree.
// They are used to extract values from data structures.
//

class SyntaxNodeExtraction : public SyntaxNode
{

    public:
        inline                  SyntaxNodeExtraction();
        inline virtual         ~SyntaxNodeExtraction();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

};

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
//
// Derivation nodes are used to represent derivation operations in the syntax tree.
// They are used to derive values from data structures.
//

class SyntaxNodeDerivation : public SyntaxNode
{

    public:
        inline                  SyntaxNodeDerivation();
        inline virtual         ~SyntaxNodeDerivation();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

};

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
//
// Unary nodes are used to represent unary operations in the syntax tree. They are
// used to evaluate unary operations and perform operations on the data.
//

class SyntaxNodeUnary : public SyntaxNode
{

    public:
        inline                  SyntaxNodeUnary();
        inline virtual         ~SyntaxNodeUnary();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Operationtype operation;
        shared_ptr<SyntaxNode> expression;

};

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
//
// Function call nodes are used to represent function calls in the syntax tree.
// They are used to call functions and pass arguments to them.
//

class SyntaxNodeFunctionCall : public SyntaxNode
{

    public:
        inline                  SyntaxNodeFunctionCall();
        inline virtual         ~SyntaxNodeFunctionCall();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        string identifier;
        vector<shared_ptr<SyntaxNode>> arguments;

};

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
//
// Array index nodes are used to represent array index operations in the syntax tree.
// They are used to access elements of an array.
//

class SyntaxNodeArrayIndex : public SyntaxNode
{

    public:
        inline                  SyntaxNodeArrayIndex();
        inline virtual         ~SyntaxNodeArrayIndex();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> array;
        vector<shared_ptr<SyntaxNode>> indices;

};

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
//
// Primary nodes are used to represent primary operations in the syntax tree. They
// are used to evaluate primary operations and perform operations on the data.
//

class SyntaxNodePrimary : public SyntaxNode
{

    public:
        inline                  SyntaxNodePrimary();
        inline virtual         ~SyntaxNodePrimary();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        Primarytype primary;
        string primitive;

};

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
//
// Grouping nodes are used to represent grouping operations in the syntax tree.
// They are used to group expressions together.
//

class SyntaxNodeGrouping : public SyntaxNode
{

    public:
        inline                  SyntaxNodeGrouping();
        inline virtual         ~SyntaxNodeGrouping();
        inline virtual void     accept(SyntaxNodeVisitor *visitor) override;

    public:
        shared_ptr<SyntaxNode> expression;

};

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

#endif