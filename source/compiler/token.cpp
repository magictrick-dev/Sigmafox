#include <compiler/token.h>

uint64_t 
token_copy_string(token *identifier, char *buffer, uint64_t buffer_size, uint64_t write_offset)
{
    
    size_t write_size = identifier->length;
    if (write_size > buffer_size - 1) write_size = buffer_size - 1;

    const char *source = identifier->source + identifier->offset + write_offset;
    for (size_t idx = 0; idx < write_size; ++idx)
        *(buffer + idx) = *(source + idx);
    buffer[write_size] = '\0';

    return write_size;

}

uint32_t
token_line_number(token *identifier)
{
    
    uint32_t line_count = 0;

    for (size_t idx = 0; idx < identifier->offset; ++idx)
        if (identifier->source[idx] == '\n') line_count++;

    // NOTE(Chris): The actual line number here is one more than that count.
    return line_count + 1;

}

uint32_t
token_column_number(token *identifier)
{
    
    size_t line_begin = identifier->offset;
    while (identifier->source[line_begin] != '\n' && line_begin != 0)
        line_begin--;

    uint32_t column_count = identifier->offset - line_begin;

    // NOTE(Chris): The acutal column number is one more than count.
    return column_count + 1;

}

/*
// NOTE(Chris): This is here because we eventually want to convert token types
//              to strings; we can modify this code to do so.
string  
parser_token_type_to_string(token *instance)
{

    switch (instance->type)
    {

        case token_type::COMMENT_BLOCK: return "comment_block";          
        case token_type::LEFT_PARENTHESIS: return "left_parenthesis";      
        case token_type::RIGHT_PARENTHESIS: return "right_parenthesis";    
        case token_type::SEMICOLON: return "semicolon";           
        case token_type::ASSIGNMENT: return "assignment";         
        case token_type::PLUS: return "plus";              
        case token_type::MINUS: return "minus";            
        case token_type::MULTIPLY: return "multiply";        
        case token_type::DIVISION: return "division";       
        case token_type::POWER: return "power";         
        case token_type::LESS_THAN: return "less_than";             
        case token_type::LESS_THAN_EQUALS: return "less_than_equals";     
        case token_type::GREATER_THAN: return "greater_than";        
        case token_type::GREATER_THAN_EQUALS: return "greater_than_equals"; 
        case token_type::EQUALS: return "equals";             
        case token_type::NOT_EQUALS: return "not_equals";        
        case token_type::CONCAT: return "concat";           
        case token_type::EXTRACT: return "extract";         
        case token_type::DERIVATION: return "derivation";     
        case token_type::IDENTIFIER: return "identifier";
        case token_type::STRING: return "string";   
        case token_type::NUMBER: return "number";
        case token_type::BEGIN: return "begin";      
        case token_type::END: return "end";
        case token_type::PROCEDURE: return "procedure";  
        case token_type::ENDPROCEDURE: return "endprocedure";
        case token_type::FUNCTION: return "function";   
        case token_type::ENDFUNCTION: return "endfunction";
        case token_type::IF: return "if";         
        case token_type::ENDIF: return "endif";
        case token_type::WHILE: return "while";      
        case token_type::ENDWHILE: return "endwhile";
        case token_type::LOOP: return "loop";       
        case token_type::ENDLOOP: return "endloop";
        case token_type::PLOOP: return "ploop";      
        case token_type::ENDPLOOP: return "endploop";
        case token_type::FIT: return "fit";        
        case token_type::ENDFIT: return "endfit";
        case token_type::VARIABLE: return "variable";
        case token_type::WRITE: return "write";
        case token_type::READ: return "read";
        case token_type::SAVE: return "save";
        case token_type::INCLUDE: return "include";
        case token_type::SCOPE: return "scope";
        case token_type::ENDSCOPE: return "endscope";
        case token_type::UNDEFINED: return "undefined";
        case token_type::END_OF_FILE: return "end_of_file";
        case token_type::END_OF_LINE: return "end_of_line";
        default: return "unset conversion";

    }

}
*/
