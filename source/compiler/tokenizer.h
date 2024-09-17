#ifndef SOURCE_COMPILER_TOKENIZER_H
#define SOURCE_COMPILER_TOKENIZER_H
#include <core/definitions.h>
#include <core/arena.h>

// --- Tokenizer ---------------------------------------------------------------
//
// The source tokenizer takes a given raw-text source file and attempts to match
// tokens that conform to the language specification. It works by examining each
// character and selectively determining if it conforms to a certain pattern.
// Think regex, except faster.
//
// The primary interface to concern yourself with is the source_tokenizer structure
// and the source_tokenizer_get_next_token(...) function. This will feed back a token,
// error or not. The main idea is that certain tokenizer errors can be tokenized
// themselves, allowing for rather accurate error reporting.
//

typedef struct source_tokenizer
{
    cc64 file_path;
    c64 source;
    u64 offset;
    u64 step;
} source_tokenizer;

typedef enum source_token_type
{
    TOKEN_COMMENT_BLOCK,
    TOKEN_LEFT_PARENTHESIS,
    TOKEN_RIGHT_PARENTHESIS,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_COLON_EQUALS,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_FORWARD_SLASH,
    TOKEN_CARROT,
    TOKEN_EQUALS,
    TOKEN_LESS_THAN,
    TOKEN_LESS_THAN_EQUALS,
    TOKEN_GREATER_THAN,
    TOKEN_GREATER_THAN_EQUALS,
    TOKEN_HASH,
    TOKEN_AMPERSAND,
    TOKEN_PIPE,
    TOKEN_PERCENT,

    TOKEN_INTEGER,
    TOKEN_REAL,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,

    TOKEN_KEYWORD_BEGIN,
    TOKEN_KEYWORD_ELSEIF,
    TOKEN_KEYWORD_END,
    TOKEN_KEYWORD_ENDFIT,
    TOKEN_KEYWORD_ENDIF,
    TOKEN_KEYWORD_ENDFUNCTION,
    TOKEN_KEYWORD_ENDLOOP,
    TOKEN_KEYWORD_ENDPLOOP,
    TOKEN_KEYWORD_ENDPROCEDURE,
    TOKEN_KEYWORD_ENDSCOPE,
    TOKEN_KEYWORD_ENDWHILE,
    TOKEN_KEYWORD_FIT,
    TOKEN_KEYWORD_FUNCTION,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_INCLUDE,
    TOKEN_KEYWORD_LOOP,
    TOKEN_KEYWORD_PLOOP,
    TOKEN_KEYWORD_PROCEDURE,
    TOKEN_KEYWORD_READ,
    TOKEN_KEYWORD_SAVE,
    TOKEN_KEYWORD_SCOPE,
    TOKEN_KEYWORD_VARIABLE,
    TOKEN_KEYWORD_WHILE,
    TOKEN_KEYWORD_WRITE,
    TOKEN_NEW_LINE,

    TOKEN_EOF = 1000,
    TOKEN_UNDEFINED,
    TOKEN_UNDEFINED_EOF,
    TOKEN_UNDEFINED_EOL,
} source_token_type;

typedef struct source_token
{
    source_token_type type;
    c64 source;
    u64 offset;
    u64 length;
} source_token;

cc64    source_token_string_nullify(source_token *token, char *hold_character);
void    source_token_string_unnullify(source_token *token, char hold_character);
void    source_token_position(source_token *token, i32 *line, i32 *col);

cc64    source_tokenizer_token_type_to_string(source_token *token);
b32     source_tokenizer_eof(source_tokenizer *tokenizer);
b32     source_tokenizer_eol(source_tokenizer *tokenizer);
b32     source_tokenizer_isalpha(source_tokenizer *tokenizer);
b32     source_tokenizer_isnum(source_tokenizer *tokenizer);
b32     source_tokenizer_isalnum(source_tokenizer *tokenizer);
b32     source_tokenizer_match(source_tokenizer *tokenizer, u32 count, ...);
char    source_tokenizer_peek(source_tokenizer *tokenizer, u64 offset);
char    source_tokenizer_consume(source_tokenizer *tokenizer, u64 count);
void    source_tokenizer_set_token(source_tokenizer *tokenizer, source_token *token, source_token_type type);
void    source_tokenizer_check_identifier(source_tokenizer *tokenizer, source_token *token);
void    source_tokenizer_synchronize(source_tokenizer *tokenizer);
b32     source_tokenizer_consume_whitespace(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_comments(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_newline(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_symbols(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_numbers(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_strings(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_identifiers(source_tokenizer *tokenizer, source_token *token);
void    source_tokenizer_get_next_token(source_tokenizer *tokenizer, source_token *token);
void    source_tokenizer_initialize(source_tokenizer *tokenizer, c64 source, cc64 path);

#endif
