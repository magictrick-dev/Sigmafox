#include <compiler/scanner.h>

// --- Source File Scanner -----------------------------------------------------
//
// The legacy method for tokenizing source files. It works by linearly going
// through the source file and peeling out a token given its valid and fits
// the syntax of the language.
//
// It uses array<type> to store this data, a C++ class being phased out.
//

bool
scanner_is_eof(scanner *state)
{
    bool eof_marker = (state->source[state->step] == '\0');
    return eof_marker;
}

bool
scanner_is_line_control(scanner *state)
{
    bool line_marker = (state->source[state->step] == '\r' || 
                        state->source[state->step] == '\n');
    return line_marker;
}

char
scanner_advance(scanner *state)
{
    char current = state->source[state->step];
    state->step++;
    return current;
}

char
scanner_peek(scanner *state)
{
    char current = state->source[state->step];
    return current;
}

uint32_t
scanner_validate_identifier_type(token *current_token)
{
    
    // Copy in the string token from the source file.
    char token_string_buffer[260];
    uint64_t write_size = token_copy_string(current_token, token_string_buffer, 260, 0);
    assert(write_size == current_token->length);

    // Force the string to lowercase for the check.
    for (size_t idx = 0; idx < write_size; ++idx)
        token_string_buffer[idx] = toupper(token_string_buffer[idx]);

    // NOTE(Chris): This is a strong candidate for a hashmap, but for now we can
    //              use a good ol' fashion double table array. We statically store
    //              this and let the first pass lazy-initialize.
    static const char*  keyword_list[21];
    static token_type   type_list[21];

    static bool list_initialized = false;
    if (list_initialized == false)
    {
        keyword_list[0] = "BEGIN";
        type_list[0] = token_type::BEGIN;
        keyword_list[1] = "END";
        type_list[1] = token_type::END;
        keyword_list[2] = "ENDPROCEDURE";
        type_list[2] = token_type::ENDPROCEDURE;
        keyword_list[3] = "ENDFUNCTION";
        type_list[3] = token_type::ENDFUNCTION;
        keyword_list[4] = "ENDIF";
        type_list[4] = token_type::ENDIF;
        keyword_list[5] = "ENDWHILE";
        type_list[5] = token_type::ENDWHILE;
        keyword_list[6] = "ENDLOOP";
        type_list[6] = token_type::ENDLOOP;
        keyword_list[7] = "ENDPLOOP";
        type_list[7] = token_type::ENDPLOOP;
        keyword_list[8] = "ENDFIT";
        type_list[8] = token_type::ENDFIT;
        keyword_list[9] = "ENDSCOPE";
        type_list[9] = token_type::ENDSCOPE;
        keyword_list[10] = "FIT";
        type_list[10] = token_type::FIT;
        keyword_list[11] = "FUNCTION";
        type_list[11] = token_type::FUNCTION;
        keyword_list[12] = "IF";
        type_list[12] = token_type::IF;
        keyword_list[13] = "INCLUDE";
        type_list[13] = token_type::INCLUDE;
        keyword_list[14] = "LOOP";
        type_list[14] = token_type::LOOP;
        keyword_list[15] = "PLOOP";
        type_list[15] = token_type::PLOOP;
        keyword_list[16] = "PROCEDURE";
        type_list[16] = token_type::PROCEDURE;
        keyword_list[17] = "READ";
        type_list[17] = token_type::READ;
        keyword_list[18] = "SAVE";
        type_list[18] = token_type::SAVE;
        keyword_list[19] = "SCOPE";
        type_list[19] = token_type::SCOPE;
        keyword_list[20] = "VARIABLE";
        type_list[20] = token_type::VARIABLE;
        keyword_list[21] = "WHILE";
        type_list[21] = token_type::WHILE;
        keyword_list[22] = "WRITE";
        type_list[22] = token_type::WRITE;
        keyword_list[23] = "ELSEIF";
        type_list[23] = token_type::ELSEIF;

        list_initialized = true;
    }

    for (size_t idx = 0; idx < 24; ++idx)
    {
        if (strcmp(keyword_list[idx], token_string_buffer) == 0)
            return type_list[idx];
    }

    return current_token->type;

}


token
scanner_create_token(scanner *state, token_type type)
{

    token result = {};
    result.source               = state->source;
    result.location             = state->file;
    result.offset               = state->start;
    result.length               = state->step - state->start;
    result.type                 = type;

    if (type == token_type::IDENTIFIER)
        result.type = scanner_validate_identifier_type(&result);

    return result;

}

bool 
scanner_tokenize_source_file(const char *source_name, const char *source_file, 
        array<token> *tokens, array<token> *errors)
{

    scanner state;
    state.file = source_name;
    state.source = source_file;
    state.step = 0;
    state.start = 0;
    state.line = 1;
    state.line_offset = 0;

    while (!scanner_is_eof(&state))
    {
        
        char c = scanner_advance(&state);
        switch (c)
        {

            // Single symbol conditions.
            case '(': tokens->push(scanner_create_token(&state, token_type::LEFT_PARENTHESIS)); break;
            case ')': tokens->push(scanner_create_token(&state, token_type::RIGHT_PARENTHESIS)); break;
            case ';': tokens->push(scanner_create_token(&state, token_type::SEMICOLON)); break;
            case '+': tokens->push(scanner_create_token(&state, token_type::PLUS)); break;
            case '-': tokens->push(scanner_create_token(&state, token_type::MINUS)); break;
            case '*': tokens->push(scanner_create_token(&state, token_type::MULTIPLY)); break;
            case '/': tokens->push(scanner_create_token(&state, token_type::DIVISION)); break;
            case '^': tokens->push(scanner_create_token(&state, token_type::POWER)); break;
            case '=': tokens->push(scanner_create_token(&state, token_type::EQUALS)); break;
            case '#': tokens->push(scanner_create_token(&state, token_type::NOT_EQUALS)); break;
            case '&': tokens->push(scanner_create_token(&state, token_type::CONCAT)); break;
            case '|': tokens->push(scanner_create_token(&state, token_type::EXTRACT)); break;
            case '%': tokens->push(scanner_create_token(&state, token_type::DERIVATION)); break;

            // White space conditions.
            case '\n': state.line++; state.line_offset = state.step; break;
            case '\t': break;
            case '\r': break;
            case ' ': break;

            // Double cases.
            case '<':
            {

                if (scanner_peek(&state) == '=')
                {
                    state.step++;
                    tokens->push(scanner_create_token(&state, token_type::LESS_THAN_EQUALS));
                }
                else
                {
                    tokens->push(scanner_create_token(&state, token_type::LESS_THAN));
                }

                break;

            };

            case '>':
            {

                if (scanner_peek(&state) == '=')
                {
                    state.step++;
                    tokens->push(scanner_create_token(&state, token_type::GREATER_THAN_EQUALS));
                }
                else
                {
                    tokens->push(scanner_create_token(&state, token_type::GREATER_THAN));
                }

                break;

            };

            case ':':
            {

                if (scanner_peek(&state) == '=')
                {
                    state.step++;
                    tokens->push(scanner_create_token(&state, token_type::ASSIGNMENT));
                    break;
                }

            };

            // Comments.
            case '{':
            {

                // Consume until '}' or EOF, whichever comes first.
                while (scanner_peek(&state) != '}' && !scanner_is_eof(&state)) state.step++;
                
                // If we reach EOF, that means there was no '}', so it is a scan error.
                if (scanner_is_eof(&state))
                {
                    errors->push(scanner_create_token(&state, token_type::END_OF_FILE));
                }

                state.start++; // We don't want to include the left bracket in the token.
                tokens->push(scanner_create_token(&state, token_type::COMMENT_BLOCK));
                state.step++;
                break;
            };

            // Quotes.
            case '\'':
            {

                // Consume characters until next '\'' or EOL or EOF.
                while (scanner_peek(&state) != '\'' &&
                        !scanner_is_line_control(&state) &&
                        !scanner_is_eof(&state))
                {
                    state.step++;
                }

                // If we hit EOL, error out.
                if (scanner_is_line_control(&state))
                {
                    errors->push(scanner_create_token(&state, token_type::END_OF_LINE));
                    return false;
                }
                
                // If we hit EOF, error out.
                if (scanner_is_eof(&state))
                {
                    errors->push(scanner_create_token(&state, token_type::END_OF_FILE));
                    return false;
                }

                state.start++;
                tokens->push(scanner_create_token(&state, token_type::STRING));
                state.step++;
                break;

            };

            // All remaining multi-character cases.
            default:
            {

                // Handle numbers and ensures if the number is a decimal, there
                // is a single trailing number at the end.
                if (isdigit(c))
                {

                    bool is_number = true;
                    while (c = scanner_peek(&state))
                    {
                        if (isdigit(c)) state.step++;
                        else if (c == '.')
                        {
                            state.step++;
                            if (!isdigit(scanner_peek(&state)))
                            {
                                is_number = false;
                                break;
                            }
                        }
                        else break;
                    }
                    
                    if (is_number == true)
                    {
                        tokens->push(scanner_create_token(&state, token_type::NUMBER));
                        break;
                    }

                }

                // Handles identifiers.
                else if (isalpha(c))
                {
                    
                    while(isalnum(c) || c == '_') c = scanner_advance(&state);
                    state.step--;
                    tokens->push(scanner_create_token(&state, token_type::IDENTIFIER));
                    break;

                }

                // Fall-through case, most likely undefined token.
                errors->push(scanner_create_token(&state, token_type::UNDEFINED));
                break;

            };

        };

        state.start = state.step;   

    }

    // The final token is an EOF marker token.
    tokens->push(scanner_create_token(&state, token_type::END_OF_FILE));
    return !(errors->size());

}

/*
// --- Tokenizer ---------------------------------------------------------------
//
// The tokenizer converts raw strings from a source file into tokens. The init
// procedure ensures that the tokenizer is set to a valid state.
//

static hash_table identifier_map;

static inline void
parser_tokenizer_initialize_identifier_map(hash_table *identifier_map)
{

    assert(identifier_map != NULL);

    // Create the hash table.
    hash_table_create(identifier_map, sizeof(uint32_t), 64, .65f, hash_function_fnv1a);

    // Store the entries using an absolutely atrocious pointer-dereferencing technique.
    *(hash_table_insert_type(identifier_map, "BEGIN", uint32_t))          = token_type::BEGIN;
    *(hash_table_insert_type(identifier_map, "END", uint32_t))            = token_type::END;
    *(hash_table_insert_type(identifier_map, "ENDPROCEDURE", uint32_t))   = token_type::ENDPROCEDURE;
    *(hash_table_insert_type(identifier_map, "ENDFUNCTION", uint32_t))    = token_type::ENDFUNCTION;
    *(hash_table_insert_type(identifier_map, "ENDIF", uint32_t))          = token_type::ENDIF;
    *(hash_table_insert_type(identifier_map, "ENDWHILE", uint32_t))       = token_type::ENDWHILE;
    *(hash_table_insert_type(identifier_map, "ENDLOOP", uint32_t))        = token_type::ENDLOOP;
    *(hash_table_insert_type(identifier_map, "ENDPLOOP", uint32_t))       = token_type::ENDPLOOP;
    *(hash_table_insert_type(identifier_map, "ENDFIT", uint32_t))         = token_type::ENDFIT;
    *(hash_table_insert_type(identifier_map, "ENDSCOPE", uint32_t))       = token_type::ENDSCOPE;
    *(hash_table_insert_type(identifier_map, "FIT", uint32_t))            = token_type::FIT;
    *(hash_table_insert_type(identifier_map, "FUNCTION", uint32_t))       = token_type::FUNCTION;
    *(hash_table_insert_type(identifier_map, "IF", uint32_t))             = token_type::IF;
    *(hash_table_insert_type(identifier_map, "INCLUDE", uint32_t))        = token_type::INCLUDE;
    *(hash_table_insert_type(identifier_map, "LOOP", uint32_t))           = token_type::LOOP;
    *(hash_table_insert_type(identifier_map, "PLOOP", uint32_t))          = token_type::PLOOP;
    *(hash_table_insert_type(identifier_map, "PROCEDURE", uint32_t))      = token_type::PROCEDURE;
    *(hash_table_insert_type(identifier_map, "READ", uint32_t))           = token_type::READ;
    *(hash_table_insert_type(identifier_map, "SAVE", uint32_t))           = token_type::SAVE;
    *(hash_table_insert_type(identifier_map, "SCOPE", uint32_t))          = token_type::SCOPE;
    *(hash_table_insert_type(identifier_map, "VARIABLE", uint32_t))       = token_type::VARIABLE;
    *(hash_table_insert_type(identifier_map, "WHILE", uint32_t))          = token_type::WHILE;
    *(hash_table_insert_type(identifier_map, "WRITE", uint32_t))          = token_type::WRITE;
    

}

static inline uint32_t
parser_tokenizer_validate_identifier_type(token *instance)
{

    char identifier_string[260];
    token_copy_string(instance, identifier_string, 260, 0);
    uint32_t *type = hash_table_find_type(&identifier_map, identifier_string, uint32_t);
    if (type != NULL) return *type;
    return instance->type;

}

static inline void
parser_tokenizer_initialize_token(tokenizer *state, token *instance, uint32_t type)
{

    instance->type      = type;
    instance->source    = state->source;
    instance->location  = state->filename;
    instance->offset    = state->offset;
    instance->length    = state->step - state->offset;
    return;

}

static inline bool
parser_tokenizer_is_eof(tokenizer *state)
{

    bool result = (state->source[state->step] == '\0');
    return result;

}

static inline bool
parser_tokenizer_is_lc(tokenizer *state)
{

    bool result (state->source[state->step] == '\r' ||
                 state->source[state->step] == '\n');
    return result;

}

static inline char
parser_tokenizer_advance(tokenizer *state)
{

    char result = state->source[state->step];
    state->step++;
    return result;

}

static inline char
parser_tokenizer_peek(tokenizer *state)
{

    char result = state->source[state->step];
    return result;

}

static inline bool
parser_tokenizer_scan_for_symbols(tokenizer *state, token *instance)
{


    char c = parser_tokenizer_advance(state);
    switch (c)
    {

        // Single symbol conditions.
        case '(':
        {
            parser_tokenizer_initialize_token(state, instance, token_type::LEFT_PARENTHESIS);
            return true;
        } break;

        case ')': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::RIGHT_PARENTHESIS);
            return true;
        } break;

        case ';': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::SEMICOLON);
            return true;
        } break;

        case '+': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::PLUS);
            return true;
        } break;

        case '-': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::MINUS);
            return true;
        } break;

        case '*': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::MULTIPLY);
            return true;
        } break;

        case '/': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::DIVISION);
            return true;
        } break;

        case '^': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::POWER);
            return true;
        } break;

        case '=': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::EQUALS);
            return true;
        } break;

        case '#': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::NOT_EQUALS);
            return true;
        } break;

        case '&': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::CONCAT);
            return true;
        } break;

        case '|': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::EXTRACT);
            return true;
        } break;

        case '%': 
        {
            parser_tokenizer_initialize_token(state, instance, token_type::DERIVATION);
            return true;
        } break;

    }

    return false;

}

static inline bool
parser_tokenizer_scan_for_comments(tokenizer *state, token *instance)
{

    assert(!"No implementation.");
    return false;

}

static inline bool
parser_tokenizer_scan_for_strings(tokenizer *state, token *instance)
{

    assert(!"No implementation.");
    return false;

}

static inline bool
parser_tokenizer_scan_for_numbers(tokenizer *state, token *instance)
{

    assert(!"No implementation.");
    return false;

}

static inline bool
parser_tokenizer_scan_for_identifiers(tokenizer *state, token *instance)
{

    assert(!"No implementation.");
    return false;

}

static void
parser_tokenizer_scan(tokenizer *state, token *instance)
{

    instance->type = token_type::UNDEFINED;
    if (!parser_tokenizer_is_eof(state))
    {

        // Scan for a token match.
        if (parser_tokenizer_scan_for_symbols(state, instance))     return;
        if (parser_tokenizer_scan_for_comments(state, instance))    return;
        if (parser_tokenizer_scan_for_strings(state, instance))     return;
        if (parser_tokenizer_scan_for_numbers(state, instance))     return;
        if (parser_tokenizer_scan_for_identifiers(state, instance)) return;

        // Generate an error token since we couldn't match a token.
        parser_tokenizer_initialize_token(state, instance, token_type::UNDEFINED);
        parser_display_error(instance, "Unrecognize symbol in source.");

        // Update the offset to the current step for the next scan.
        state->offset = state->step;

    }

}

void
parser_tokenizer_initialize(tokenizer *state, const char *source, const char *filename)
{

    assert(state != NULL);
    state->source   = source;
    state->filename = filename;
    state->step     = 0;
    state->offset   = 0;
    return;

}

bool    
parser_tokenizer_consume_token(tokenizer *state, token *instance)
{

    if (parser_tokenizer_is_eof(state)) false;
    parser_tokenizer_scan(state, instance);

    bool result = (instance->type != token_type::UNDEFINED);
    return result;

}

bool    
parser_tokenizer_consume_all_tokens(tokenizer *state, token *tokens, uint64_t *count)
{

    assert(!"No implementation.");
    return false;
}
*/
