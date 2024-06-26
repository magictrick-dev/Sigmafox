// --- Parser ------------------------------------------------------------------
//
// Contains the required methods to parse a source file into an AST list. In
// order to properly transform a source file into an AST, we first tokenize the
// source file into a set of consumable tokens, then we take this list of tokens
// and construct an AST.
//

#ifndef SIGMAFOX_COMPILER_PARSER_H 
#define SIGMAFOX_COMPILER_PARSER_H 
#include <core/utilities.h>
#include <core/definitions.h>
#include <compiler/token.h>

#define PARSER_VARIABLE_MAX_DIMENSIONS 4

enum class statement_type
{
    STATEMENT,
    COMMENT_STATEMENT,
    EXPRESSION_STATEMENT,
    BLOCK_STATEMENT,
    DECLARATION_STATEMENT,
};

enum class expression_type 
{
    EXPRESSION,
    ASSIGNMENT,
    EQUALITY,
    COMPARISON,
    TERM,
    FACTOR,
    UNARY,
    PRIMARY
};

enum class ast_node_type
{

    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
    GROUPING_EXPRESSION,
    LITERAL_EXPRESSION,

    EXPRESSION_STATEMENT,
    COMMENT_STATEMENT,
    DECLARATION_STATEMENT,
    BLOCK_STATEMENT,
    ASSIGNMENT_STATEMENT,

};

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

        struct comment_statement
        {
            token *comment;
        } comment_statement;

    };

};

bool    parser_tokenize_source_file(const char *file, const char *source,
            array<token> *tokens, array<token> *errors);

bool parser_generate_abstract_syntax_tree(array<token> *tokens, 
        array<statement*> *statements, memory_arena *arena);
void parser_ast_traversal_print(array<statement*> *statements);

#endif
