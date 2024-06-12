#include <cctype>
#include <scan.h>

// --- Internal Helpers --------------------------------------------------------
//
// The scanner requires some helper functions to reduce overall code complexity
// in the scan function. The scanner state helps pass information regarding
// where the scanner is at so parameter definitions aren't as long.
//

struct scanner_state
{
    const char *source;
    size_t step;
    size_t start;
    size_t line;
};

static inline bool
scanner_is_eof(scanner_state *state)
{
    bool eof_marker =  (state->source[state->step] == '\0'   ||
                        state->source[state->step] == '\13'  ||
                        state->source[state->step] == '\10');
    return eof_marker;
}

static inline bool
scanner_is_line_control(scanner_state *state)
{
    bool line_marker = (state->source[state->step] == '\r' || 
                        state->source[state->step] == '\n');
    return line_marker;
}

static inline char
scanner_advance(scanner_state *state)
{
    char current = state->source[state->step];
    state->step++;
    return current;
}

static inline char
scanner_peek(scanner_state *state)
{
    char current = state->source[state->step];
    return current;
}

static inline token
scanner_create_token(scanner_state *state, token_type type)
{

    token result = {};
    result.source = state->source;
    result.offset = state->start;
    result.length = state->step - state->start;
    result.line = state->line;
    result.type = type;
    return result;

}

// --- Scan Source File --------------------------------------------------------
//
// Sequentially scans a source file by inspecting leading characters and properly
// matching expressions into tokens.
//

bool 
scanner_scan_source_file(const char *source_file, array<token> *tokens, array<token> *errors)
{

    scanner_state state;
    state.source = source_file;
    state.step = 0;
    state.start = 0;
    state.line = 1;

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
            case '\n': state.line++; break;
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
                    tokens->push(scanner_create_token(&state, token_type::GREATER_THAN_EQUALS));
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

