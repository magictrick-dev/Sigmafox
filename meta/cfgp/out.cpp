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
        inline void visit_binary(ExpressionVisitor visitor);
        inline void visit_grouping(ExpressionVisitor visitor);
        inline void visit_literal(ExpressionVisitor visitor);
        inline void visit_unary(ExpressionVisitor visitor);
};

// --- Binary ------------------------------------------------------------------

class Binary : public Expression
{
    public:
        inline virtual void accept(ExpressionVisitor visitor) override
        { visitor.visit_binary(this); }

    protected:
        Expression left;
        Token op;
        Expression right;

};

// --- Grouping ----------------------------------------------------------------

class Grouping : public Expression
{
    public:
        inline virtual void accept(ExpressionVisitor visitor) override
        { visitor.visit_grouping(this); }

    protected:
        Expression exp;

};

// --- Literal -----------------------------------------------------------------

class Literal : public Expression
{
    public:
        inline virtual void accept(ExpressionVisitor visitor) override
        { visitor.visit_literal(this); }

    protected:
        Token value;

};

// --- Unary -------------------------------------------------------------------

class Unary : public Expression
{
    public:
        inline virtual void accept(ExpressionVisitor visitor) override
        { visitor.visit_unary(this); }

    protected:
        Token op;
        Expression right;

};

#endif