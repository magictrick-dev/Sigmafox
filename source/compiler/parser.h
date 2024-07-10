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

    EXPRESSION_STATEMENT,
    COMMENT_STATEMENT,
    DECLARATION_STATEMENT,
    BLOCK_STATEMENT,
    ASSIGNMENT_STATEMENT,
    WHILE_STATEMENT,
    IF_STATEMENT,

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

enum class statement_type
{
    STATEMENT,
    COMMENT_STATEMENT,
    EXPRESSION_STATEMENT,
    BLOCK_STATEMENT,
    DECLARATION_STATEMENT,
    WHILE_STATEMENT,
    IF_STATEMENT,
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

#endif
