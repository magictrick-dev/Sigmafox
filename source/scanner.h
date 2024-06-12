#ifndef SIGMAFOX_SCANNER_H
#define SIGMAFOX_SCANNER_H
#include <core/definitions.h>

enum token_type
{
    comment_block,          // { ... }
    left_parenthesis,       // (
    right_parenthesis,      // )
    semicolon,              // ;
    assignment,             // :=
    plus,                   // +
    minus,                  // -
    multiply,               // *
    division,               // /
    power,                  // ^
    less_than,              // <
    less_than_equals,       // <=
    greater_than,           // >
    greater_than_equals,    // >=
    equals,                 // =
    not_equals,             // #
    concat,                 // &
    extract,                // |
    derivation,             // %

    identifier,
    string,
    number,

    begin,
    end,
    procedure,
    endprocedure,
    function,   
    endfunction,
    if,         
    endif,
    while,      
    endwhile,
    loop,       
    endloop,
    ploop,      
    endploop,
    fit,        
    endfit,
    variable,
    write,
    read,
    save,
    include,

    print, // Temporary keyword, not in language spec.

    undefined,
    eof
};

struct token
{
    const char *source;
    size_t offset;
    size_t length;
    size_t line;
    uint32_t type;
};

struct token_node
{
    token_node *next;
    token_node *prev;

    token lexeme;
}; 

struct token_node_list
{
    token_node *root;
    size_t count;
};

inline bool 
scanner_scan_source_file(const char *source_file, token_node_list *tokens, token_node_list *errors)
{

}

#endif
