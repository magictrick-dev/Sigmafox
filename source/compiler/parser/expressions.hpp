#ifndef SIGMAFOX_COMPILER_PARSER_EXPRESSIONS_HPP
#define SIGMAFOX_COMPILER_PARSER_EXPRESSIONS_HPP
#include <definitions.hpp>
#include <compiler/parser/node.hpp>

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
        shared_ptr<SyntaxNode> expression;

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
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

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
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

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
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

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
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

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
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

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
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

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
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

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
        shared_ptr<SyntaxNode> left;
        shared_ptr<SyntaxNode> right;

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
        shared_ptr<SyntaxNode> expression;

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
        vector<shared_ptr<SyntaxNode>> arguments;

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
        vector<shared_ptr<SyntaxNode>> indices;

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
        Primarytype primary;
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
        shared_ptr<SyntaxNode> expression;

};

#endif