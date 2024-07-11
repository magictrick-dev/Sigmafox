#ifndef SOURCE_COMPILER_STATEMENTS_H
#define SOURCE_COMPILER_STATEMENTS_H
#include <core/definitions.h>
#include <core/utilities.h>
#include <compiler/parser.h>
#include <compiler/expressions.h>

struct statement
{
    ast_node_type node_type;

    union
    {

        struct expression_statement
        {
            expression *expr;
        } expression_statement;

        struct declaration_statement
        {
            token *identifier;
            expression *size;

            size_t dimension_count;
            expression* dimensions[PARSER_VARIABLE_MAX_DIMENSIONS];
        } declaration_statement;

        struct assignment_statement
        {
            token *identifier;
            expression *value;
        } assignment_statement;

        struct block_statement
        {
            linked_list statements;
        } block_statement;

        struct while_statement
        {
            linked_list statements;
            expression *check;
        } while_statement;

        struct for_statement
        {
            token *identifier;
            expression *start;
            expression *end;
            expression *increment;
            linked_list statements;
        } for_statement;

        struct if_statement
        {
            expression *if_check;
            linked_list if_block;
            linked_list elseif_statements;
        } if_statement;

        struct elseif_statement
        {
            expression *elseif_check;
            linked_list elseif_block;
        } elseif_statement;

        struct comment_statement
        {
            token *comment;
        } comment_statement;

    };

};

statement* parser_recursively_descend_statement(parser_state *state, statement_type level);

#endif
