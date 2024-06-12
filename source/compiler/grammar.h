#ifndef SIGMAFOX_COMPILER_GRAMMAR_H
#define SIGMAFOX_COMPILER_GRAMMAR_H
#include <string>
#include <iostream>
#include <allocator.h>
#include <compiler/token.h>

// --- Formal Grammar of Expression Syntax -----------------------------
//
// COSY's operator precendence is slightly different than the one we are
// using here. The changes only affect equality (equal to, not equal to)
// and comparison (less than, greater than) in the sense that comparisons
// proceed equality. This makes logical sense and obeys the C-standard that
// will ultimately be used once the language is transpiled.
//
// These aren't all the operators within the language, but the ones we
// want to focus on for now:
//
// Expression   (1) 
// Equality     (2) [ = # ]
// Comparison   (3) [ < > <= >= ]
// Term         (5) [ + - ]
// Factor       (6) [ / * ]
// Unary        (7) [ - ]
// Primary      (8) Literals
//
// Context Free Grammar Representation for Expressions:
//
// expression   : equality
// equality     : comparison ( ( "=" | "#" ) comparison )*
// comparison   : term ( ( "<" | ">" | "<=" | ">=" ) term )*
// term         : factor ( ( "+" | "-" ) factor )*
// factor       : unary ( ( "*" | "/" ) unary )*
// unary        : ( "-" ) unary | primary
// primary      : NUMBER | STRING | "true" | "false" | "(" expression ")"
//
// Now, to get actual side effects, we need to turn expressions to statements.
//
// program              : statement* EOF
// statement            : expressionstatement | printstatement
// expressionstatement  : expression ";"
// printstatement       : "print" expression ";"
//

enum class
LiteralType
{
    NUMBER,
    STRING,
    BOOLEAN,
};

class Expression;
class BinaryExpression;
class GroupingExpression;
class LiteralExpression;
class UnaryExpression;

// --- Expressions -----------------------------------------------------

class ExpressionVisitor
{
    public:
        virtual void visit_binary_expression(BinaryExpression *expr)      = 0;
        virtual void visit_grouping_expression(GroupingExpression *expr)  = 0;
        virtual void visit_literal_expression(LiteralExpression *expr)    = 0;
        virtual void visit_unary_expression(UnaryExpression *expr)        = 0;
};

class Expression
{
    public:
        inline virtual void         accept_visitor(ExpressionVisitor *visitor) = 0;
        inline virtual std::string  to_string() = 0;
};

class BinaryExpression : public Expression
{
    public:
        inline 
        BinaryExpression(sharedptr<Expression> left, Token op, sharedptr<Expression> right)
        {
            this->left      = left;
            this->operation = op;
            this->right     = right;
        }

        inline virtual void 
        accept_visitor(ExpressionVisitor *visitor)
        {
            visitor->visit_binary_expression(this);  
        };

        inline virtual std::string to_string()
        {
            return left->to_string() + 
                " " + operation.lexeme +
                " " + right->to_string();
        }

    public:
        Token                   operation;
        sharedptr<Expression>   left;
        sharedptr<Expression>   right;
};

class GroupingExpression : public Expression
{
    public:
        inline 
        GroupingExpression(sharedptr<Expression> expression)
        {
            this->expression = expression;
        }

        inline virtual void
        accept_visitor(ExpressionVisitor *visitor)
        {
            visitor->visit_grouping_expression(this);
        }

        inline virtual std::string
        to_string()
        {
            return "( " + expression->to_string() + " )";
        }

    public:
        sharedptr<Expression> expression;
};

class UnaryExpression : public Expression
{
    public:
        inline 
        UnaryExpression(Token operation, sharedptr<Expression> expression)
        {
            this->operation     = operation;
            this->expression    = expression;
        }

        inline virtual void
        accept_visitor(ExpressionVisitor *visitor)
        {
            visitor->visit_unary_expression(this);
        }

        inline virtual std::string
        to_string()
        {
            return operation.lexeme + " " + expression->to_string();
        }

    public:
        Token                       operation;
        std::shared_ptr<Expression> expression;
};

class LiteralExpression : public Expression
{
    public:
        inline 
        LiteralExpression(std::string literal, LiteralType type)
        {
            this->literal   = literal;
            this->type      = type;
        }

        inline virtual void
        accept_visitor(ExpressionVisitor *visitor)
        {
            visitor->visit_literal_expression(this);
        }

        inline virtual std::string to_string()
        {
            if (this->type == LiteralType::STRING)
                return "\"" + this->literal + "\"";
            return this->literal;
        }

    public:
        std::string literal;
        LiteralType type;

};

// --- Statements ------------------------------------------------------

class Statement;
class ExpressionStatement;
class PrintStatement;

class StatementVisitor 
{
    public:
        virtual void visit_expression(ExpressionStatement *stm) = 0;
        virtual void visit_print(PrintStatement *stm)           = 0;
};

class Statement
{
    public:
        inline virtual void         accept_visitor(StatementVisitor *visitor) = 0;
};

class ExpressionStatement : public Statement
{

    public:
        inline
        ExpressionStatement(sharedptr<Expression> expr)
        {
            this->expr = expr;
        }

        inline virtual void
        accept_visitor(StatementVisitor *visitor)
        {
            visitor->visit_expression(this);
        }
    
    public:
        sharedptr<Expression> expr;

};

class PrintStatement : public Statement
{
    public:
        inline 
        PrintStatement(sharedptr<Expression> expr)
        {
            this->expr = expr;
        }

        inline virtual void
        accept_visitor(StatementVisitor *visitor)
        {
            visitor->visit_print(this);
        }

    public:
        sharedptr<Expression> expr;
};

// --- Visitor Implementations -----------------------------------------
//
// Below are some visitor implementations.
//

class ExpressionPrinter : public ExpressionVisitor
{

    public:
        bool wrap = false;

    protected:
        virtual void 
        visit_binary_expression(BinaryExpression *expr)
        {
            if (wrap) std::cout << "( ";
            //std::cout << expr->to_string();   
            expr->left->accept_visitor(this);
            std::cout << " " << expr->operation.lexeme << " ";
            expr->right->accept_visitor(this);
            if (wrap) std::cout << " )";
        }

        virtual void
        visit_grouping_expression(GroupingExpression *expr)
        {
            expr->expression->accept_visitor(this);
        }

        virtual void
        visit_literal_expression(LiteralExpression *expr)
        {
            std::cout << expr->to_string();   
        }

        virtual void
        visit_unary_expression(UnaryExpression *expr)
        {
            if (wrap) std::cout << "( ";
            std::cout << expr->to_string();   
            if (wrap) std::cout << " )";
        }

};

class StatementPrinter : public StatementVisitor, ExpressionVisitor
{

    public:
        bool wrap = false;

    protected:
        virtual void 
        visit_binary_expression(BinaryExpression *expr)
        {
            if (wrap) std::cout << "( ";
            //std::cout << expr->to_string();   
            expr->left->accept_visitor(this);
            std::cout << " " << expr->operation.lexeme << " ";
            expr->right->accept_visitor(this);
            if (wrap) std::cout << " )";
        }

        virtual void
        visit_grouping_expression(GroupingExpression *expr)
        {
            expr->expression->accept_visitor(this);
        }

        virtual void
        visit_literal_expression(LiteralExpression *expr)
        {
            std::cout << expr->to_string();   
        }

        virtual void
        visit_unary_expression(UnaryExpression *expr)
        {
            if (wrap) std::cout << "( ";
            std::cout << expr->to_string();   
            if (wrap) std::cout << " )";
        }

    protected:
        virtual void 
        visit_expression(ExpressionStatement *stm)
        {
            stm->expr->accept_visitor(this);
            std::cout << ";" << std::endl;
        }

        virtual void 
        visit_print(PrintStatement *stm)
        {
            std::cout << "std::cout << ";
            stm->expr->accept_visitor(this);
            std::cout << " << std::endl;" << std::endl;
        }

};

#endif
