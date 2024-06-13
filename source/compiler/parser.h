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

#endif
