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
    PROCEDURE,  
    ENDPROCEDURE,
    FUNCTION,   
    ENDFUNCTION,
    IF,         
    ENDIF,
    WHILE,      
    ENDWHILE,
    LOOP,       
    ENDLOOP,
    PLOOP,      
    ENDPLOOP,
    FIT,        
    ENDFIT,
    VARIABLE,
    WRITE,
    READ,
    SAVE,
    INCLUDE,

    // Special:
    PRINT,
    UNDEFINED,
    END_OF_FILE,
    END_OF_LINE,

};

struct token
{
    const char *source;
    size_t offset_from_start;
    size_t offset_from_line;
    size_t length;
    size_t line;
    token_type type;
};

bool    parser_tokenize_source_file(const char *source, array<token> *tokens, array<token> *errors);
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

enum node_type
{
    BINARY_NODE,
    GROUPING_NODE,
    UNARY_NODE,
    LITERAL_NODE,
};

struct ast_node
{

    int node_type;

    // We union operation and literal token pointers, they
    // point to the same data but have logically different
    // meanings depending on the node type.
    union
    {
        token *operation;

        struct
        {
            token *literal;
        };
    };

    // Assume that an ast_node is binary at most, so we 
    // union left/right, and let expression/expr be unary.
    // For leaf nodes, we assume null.
    union
    {
        ast_node *nodes[2];

        struct
        {
            ast_node *expression;
            ast_node *right_void;
        };

        struct
        {
            ast_node *left_expression;
            ast_node *right_expression;
        };
    };

};

enum class statement_type
{
    STATEMENT,
    EXPRESSION_STATEMENT,
};

enum class expression_type 
{
    EXPRESSION,
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
    GROUPING_EXPRESSION,
    LITERAL_EXPRESSION,

    EXPRESSION_STATEMENT,
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

    };

};

ast_node* parser_create_ast_binary_node(ast_node *left, ast_node *right, token *operation);
ast_node* parser_create_ast_grouping_node(ast_node *expression);
ast_node* parser_create_ast_unary_node(ast_node *expression, token *operation);
ast_node* parser_create_ast_literal_node(token *literal);

bool parser_ast_create(array<token> *tokens, ast_node **ast, array<void*> *alloc_list);
void parser_ast_free_traversal(ast_node *ast);
void parser_ast_print_traversal(ast_node *ast);
void parser_ast_print_order_traversal(ast_node *ast);

bool parser_generate_abstract_syntax_tree(array<token> *tokens, array<statement*> *statements, memory_arena *arena);
void parser_ast_traversal_print(array<statement*> *statements);

#endif
