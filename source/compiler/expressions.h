#ifndef SOURCE_COMPILER_EXPRESSIONS_H
#define SOURCE_COMPILER_EXPRESSIONS_H
#include <core/definitions.h>
#include <compiler/parser.h>

struct expression
{

    ast_node_type node_type;
 
    union
    {

        struct binary_expression
        {
            expression *left;
            expression *right;
            token *literal;
        } binary_expression;

        struct unary_expression
        {
            expression *primary;
            token *literal;
        } unary_expression;

        struct assignment_expression
        {
            expression *assignment;
            token *identifier;
        } assignment_expression;

    };

};

expression* parser_recursively_descend_expression(parser_state *state, expression_type level);

#endif
