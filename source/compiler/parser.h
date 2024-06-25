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

#define PARSER_VARIABLE_MAX_DIMENSIONS 4

// --- Tokenizer ---------------------------------------------------------------
//
// The list of definitions regarding the tokenizer stage of the parser.
//

enum class token_type
{

    // Symbols:
    COMMENT_BLOCK,          // { ... }
    LEFT_PARENTHESIS,       // (
    RIGHT_PARENTHESIS,      // )
    SEMICOLON,              // ;
    ASSIGNMENT,             // :=
    PLUS,                   // +
    MINUS,                  // -
    MULTIPLY,               // *
    DIVISION,               // /
    POWER,                  // ^
    LESS_THAN,              // <
    LESS_THAN_EQUALS,       // <=
    GREATER_THAN,           // >
    GREATER_THAN_EQUALS,    // >=
    EQUALS,                 // =
    NOT_EQUALS,             // #
    CONCAT,                 // &
    EXTRACT,                // |
    DERIVATION,             // %

    // Definables: 
    IDENTIFIER,
    STRING,                 // '', single quotes only.
    NUMBER,

    // Keywords: 
    BEGIN,      
    END,
    ENDPROCEDURE,
    ENDFUNCTION,
    ENDIF,
    ENDWHILE,
    ENDLOOP,
    ENDPLOOP,
    ENDFIT,
    ENDSCOPE,
    FIT,        
    FUNCTION,   
    IF,         
    INCLUDE,
    LOOP,       
    PLOOP,      
    PROCEDURE,  
    READ,
    SAVE,
    SCOPE,
    VARIABLE,
    WHILE,      
    WRITE,

    // Special:
    UNDEFINED,
    END_OF_FILE,
    END_OF_LINE,

};

struct token
{
    const char *source;
    const char *location;
    size_t offset_from_start;
    size_t offset_from_line;
    size_t length;
    size_t line;
    token_type type;
};

bool    parser_tokenize_source_file(const char *file, const char *source,
            array<token> *tokens, array<token> *errors);
string  parser_token_to_string(token *instance);
string  parser_token_type_to_string(token *instance);

// --- AST ---------------------------------------------------------------------
//
// The collection of definitions for the AST portion of the parser. When a node
// is created, it either contains 0, 1, or 2 branch nodes (ternary is not supported).
// We combine this functionality into a single ast_node structure with unioned nodes.
// Data orientation is only relavent during traversal time, and in such cases the
// type of node is known.
//
// When an AST is generated, you can optionally create an allocation list. This
// allocation list allows for easy-deallocation of the AST without having to write
// a traversal routine to do so.
//
// The parser will return false if there are any syntax errors.
//

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

bool parser_generate_abstract_syntax_tree(array<token> *tokens, 
        array<statement*> *statements, memory_arena *arena);
void parser_ast_traversal_print(array<statement*> *statements);

#endif
