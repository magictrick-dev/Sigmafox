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

enum token_type
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
    uint32_t type;
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
            ast_node *expr;
        };

        struct
        {
            ast_node *left_expression;
            ast_node *right_expressions;
        };
    };

};

ast_node* create_ast_node(node_type type, token *instance, array<void*> *alloc_list);
ast_node* create_ast_binary_node(ast_node *left, ast_node *right, token *operation);
ast_node* create_ast_grouping_node(ast_node *expression);
ast_node* create_ast_unary_node(ast_node *expression, token *operation);
ast_node* create_ast_literal_node(token *literal);

bool parser_create_ast(array<token> *tokens, ast_node **ast, array<void*> *alloc_list);

#endif
