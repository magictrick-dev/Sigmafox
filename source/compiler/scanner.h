#ifndef SOURCE_COMPILER_SCANNER_H
#define SOURCE_COMPILER_SCANNER_H
#include <core/definitions.h>
#include <core/utilities.h>
#include <compiler/token.h>

typedef struct scanner
{
    const char *source;
    const char *file;
    size_t step;
    size_t start;
    size_t line;
    size_t line_offset;
} scanner;

bool        scanner_is_eof(scanner *state);
bool        scanner_is_line_control(scanner *state);
char        scanner_advance(scanner *state);
char        scanner_peek(scanner *state);
uint32_t    scanner_validate_identifier_type(token *current_token);
token       scanner_create_token(scanner *state, token_type type);
bool        scanner_tokenize_source_file(const char *source, const char *file, 
                array<token> *tokens, array<token> *errors);

// --- Tokenizer ---------------------------------------------------------------
//
// The tokenizer's responsibility is to take the raw textual input from a source
// file and categorize each lexeme into its grammatical representation. The tokenizer
// works in linear order, and therefore it can be used to tokenize an entire source
// file at once, or to collect a token one at a time. The AST can use this one
// at a time functionality to fetch tokens as they're needed, storing them as
// required since most tokens do not need to persist.
//

/*
typedef struct tokenizer
{
    const char *source;
    const char *filename;
    uint64_t    step;
    uint64_t    offset;
} tokenizer;

void    parser_tokenizer_initialize(tokenizer *state, const char *source, const char *filename);
bool    parser_tokenizer_consume_token(tokenizer *state, token *instance);
bool    parser_tokenizer_consume_all_tokens(tokenizer *state, token *tokens, uint64_t *count);
*/

#endif
