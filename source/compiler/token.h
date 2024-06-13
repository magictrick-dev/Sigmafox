#ifndef SIGMAFOX_COMPILER_TOKEN_H
#define SIGMAFOX_COMPILER_TOKEN_H
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
    size_t offset;
    size_t length;
    size_t line;
    uint32_t type;
};

inline string
token_to_string(token instance)
{
    string result(instance.length + 1);
    for (size_t idx = 0; idx < instance.length; ++idx)
        result[idx] = instance.source[instance.offset + idx];
    return result;
}

#endif
