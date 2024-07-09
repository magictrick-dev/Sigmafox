#ifndef SIGMAFOX_COMPILER_TOKEN_H
#define SIGMAFOX_COMPILER_TOKEN_H
#include <core/definitions.h>

typedef enum token_type : uint32_t 
{

    COMMENT_BLOCK,   
    LEFT_PARENTHESIS, 
    RIGHT_PARENTHESIS, 
    SEMICOLON, 
    ASSIGNMENT, 
    PLUS,       
    MINUS,       
    MULTIPLY,     
    DIVISION,      
    POWER,          
    LESS_THAN,       
    LESS_THAN_EQUALS, 
    GREATER_THAN,      
    GREATER_THAN_EQUALS,
    EQUALS,            
    NOT_EQUALS,         
    CONCAT,              
    EXTRACT,              
    DERIVATION,            

    IDENTIFIER,
    STRING,                 
    NUMBER,

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

    UNDEFINED,
    END_OF_FILE,
    END_OF_LINE,

} token_type;

typedef struct token
{
    const char *source;
    const char *location;
    size_t offset;
    size_t length;
    uint32_t type;
} token;

uint64_t token_copy_string(token *identifier, char *buffer, uint64_t buffer_size, uint64_t write_offset);
//uint64_t token_copy_string_type(token *identifier, char *buffer, uint64_t buffer_size, uint64_t write_offset);
uint32_t token_line_number(token *identifier);
uint32_t token_column_number(token *identifier);

#endif
