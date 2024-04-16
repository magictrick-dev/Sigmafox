// This file was partially generated using a metaprogram, cgfp.
// See ./meta/cgfp for details.

#ifndef SIGMAFOX_EXPRESSION_H
#define SIGMAFOX_EXPRESSION_H

// --- Expression --------------------------------------------------------------

class Expression
{
    public:
        virtual void accept(ExpressionVisitor) = 0;
};

// --- ExpressionVisitor -------------------------------------------------------

class ExpressionVisitor
{
    public:
        inline void visit_binary(ExpressionVisitor visitor) {}

        inline void visit_grouping(ExpressionVisitor visitor) {}

        inline void visit_literal(ExpressionVisitor visitor) {}

        inline void visit_unary(ExpressionVisitor visitor) {}

};

// --- Binary ------------------------------------------------------------------

class Binary : public Expression
{
    protected:
        Expression left;
        Token op;
        Expression right;

};

// --- Grouping ----------------------------------------------------------------

class Grouping : public Expression
{
    protected:
        Expression exp;

};

// --- Literal -----------------------------------------------------------------

class Literal : public Expression
{
    protected:
        Token value;

};

// --- Unary -------------------------------------------------------------------

class Unary : public Expression
{
    protected:
        Token op;
        Expression right;

};

#endif