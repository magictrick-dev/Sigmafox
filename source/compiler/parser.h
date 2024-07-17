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
#include <compiler/environment.h>

#define PARSER_VARIABLE_MAX_DIMENSIONS 4


// --- Seperation Refactor -----------------------------------------------------

enum class ast_node_type
{

    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
    GROUPING_EXPRESSION,
    LITERAL_EXPRESSION,
    CALL_EXPRESSION,

    EXPRESSION_STATEMENT,
    COMMENT_STATEMENT,
    DECLARATION_STATEMENT,
    BLOCK_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    IF_STATEMENT,
    ELSEIF_STATEMENT,
    PROCEDURE_STATEMENT,
    INVOKEPROC_STATEMENT,
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
    CALL,
    PRIMARY
};

typedef struct parser_state
{
    bool errored;
    size_t current;
    array<token> *tokens;
    memory_arena *arena;
    environment global_environment;
} parser_state;

bool    parser_validate_token(parser_state *state, token_type type);
bool    parser_check_token(parser_state *state, token_type type);
bool    parser_match_token(parser_state *state, token_type type);
token*  parser_consume_token(parser_state *state, token_type type);
token*  parser_get_current_token(parser_state *state);
token*  parser_get_previous_token(parser_state *state);
void    parser_synchronize_state(parser_state *state);

#ifndef PROPAGATE_DEFINED
#define PROPAGATE_DEFINED
#define propagate_on_error(exp) if ((exp) == NULL) return NULL

#define ERROR_CODE_TOKENIZER_UNDEFINED_SYMBOL       "0x0001000A"
#define ERROR_CODE_TOKENIZER_EOL_REACH              "0x0001000B"
#define ERROR_CODE_TOKENIZER_EOF_REACH              "0x0001000C"
#define ERROR_CODE_SYNTAX_NO_SEMICOLON              "0x001000A0"
#define ERROR_CODE_SYNTAX_UNEXPECTED_SYMBOL         "0x001000B0"
#define ERROR_CODE_SYNTAX_MISSING_SYMBOL            "0x001000C0"
#define ERROR_CODE_SYNTAX_INVALID_EXPRESSION        "0x01000A00"
#endif

void    parser_display_error(token *location, const char *reason);
void    parser_display_warning(token *location, const char *reason);

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

        struct call_expression
        {
            token *identifier;
            linked_list parameter_expressions;
        } call_expression;

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
            linked_list if_block; // Statements
            linked_list elseif_statements; // elseif_statementnodes
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

        struct procedure_statement
        {
            token *identifier;              // Name of procedure.
            linked_list statements;         // Statement body.
            linked_list parameter_names;    // Parameter names.
        } procedure_statement;

    };

};

statement* parser_create_comment_statement(parser_state *state);
statement* parser_create_declaration_statement(parser_state *state);
statement* parser_create_block_statement(parser_state *state);
statement* parser_create_if_statement(parser_state *state);
statement* parser_create_elseif_statement(parser_state *state);
statement* parser_create_while_statement(parser_state *state);
statement* parser_create_for_statement(parser_state *state);
statement* parser_create_expression_statement(parser_state *state);
statement* parser_create_procedure_statement(parser_state *state);

statement*  parser_recursively_descend_statement(parser_state *state);
expression* parser_recursively_descend_expression(parser_state *state, expression_type level);

// --- Parser Refactor Complete ------------------------------------------------
//
// A complete refactor of the syntax nodes for AST generation. This allows for
// complete contiguous association of syntax nodes for CPU cache efficiency. This
// design should accomodate linked-list behaviors by making each referenceable
// object of type syntax_node. This means that we can freely use a memory arena
// to handle each stage of the parser.
//
// The tokenizer will be refactor appropriately to handle on-demand fetching. The
// parser no longer maintains references of tokens, only consumes them. Therefore,
// we store primitives within a syntax node and larger data types like strings in
// a pool. This pool will exist separately from the AST and thus will be fixed-size.
//

struct syntax_node;
struct binary_syntax_node;
struct unary_syntax_node;
struct grouping_syntax_node;
struct primary_syntax_node;
struct call_syntax_node;

typedef enum syntax_node_type : uint32_t
{
    BINARY_EXPRESSION_NODE,
    UNARY_EXPRESSION_NODE,
    PRIMARY_EXPRESSION_NODE,
    CALL_EXPRESSION_NODE,
    GROUPING_EXPRESSION_NODE,

    EXPRESSION_STATEMENT_NODE,
    COMMENT_STATEMENT_NODE,
    NEWLINE_STATEMENT_NODE,
    DECLARATION_STATEMENT_NODE,
    SCOPE_STATEMENT_NODE,
    LOOP_STATEMENT_NODE,
    WHILE_STATEMENT_NODE,
    IF_STATEMENT_NODE,
    ELSEIF_STATEMENT_NODE,
    PROCEDURE_STATEMENT_NODE
} syntax_node_type;

typedef enum syntax_operation_type : uint32_t
{
    OPERATION_ADDITION,
    OPERATION_SUBTRACTION,
    OPERATION_MULTIPLICATION,
    OPERATION_DIVISION,

    OPERATION_NEGATIVE_ASSOCIATE,
} syntax_operation_type;

typedef union object_literal
{

    uint64_t        unsigned_integer;
    int64_t         signed_integer;
    int64_t         boolean;
    double          real;
    const char     *string;
    const char     *identifier;
    const char     *procedure_name;

} object_literal;

typedef enum object_type
{
    OBJECT_UNSIGNED_INTEGER,
    OBJECT_SIGNED_INTEGER,
    OBJECT_BOOLEAN,
    OBJECT_REAL,
    OBJECT_STRING,
    OBJECT_PROCEDURE,
    OBJECT_IDENTIFIER,
} object_type;

typedef struct binary_syntax_node
{
    syntax_node *left;
    syntax_node *right;
    syntax_operation_type type;
} binary_syntax_node;

typedef struct unary_syntax_node
{
    syntax_node *right;
    syntax_operation_type type;
} unary_syntax_node;

typedef struct grouping_syntax_node
{
    syntax_node *grouping;
} grouping_syntax_node;

typedef struct primary_syntax_node
{
    object_literal literal;
    object_type type;
} primary_syntax_node;

typedef struct parameter_syntax_node
{
    syntax_node *expression;
    syntax_node *next_parameter;
} parameter_syntax_node;

typedef struct call_syntax_node 
{
    object_literal call_identifier;
    object_type call_type;
    syntax_node *parameter_list;
    uint32_t parameter_count; 
} call_syntax_node;

typedef struct assigment_syntax_node
{
    syntax_node *expression;
    object_literal literal;
    object_type type;
} assignment_syntax_node;

typedef struct syntax_node
{
    syntax_node_type type;

    union
    {
        binary_syntax_node      binary;
        unary_syntax_node       unary;
        primary_syntax_node     primary;
        grouping_syntax_node    grouping;
        call_syntax_node        call;
        parameter_syntax_node   parameter;
        assignment_syntax_node  assignment;
    };

} syntax_node;

#endif
