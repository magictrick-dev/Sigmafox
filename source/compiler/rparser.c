#include <compiler/rparser.h>

// --- Tokenizer ---------------------------------------------------------------
//
// The tokenizer is responsible for converting a raw-text source file into tokens.
// The majority of this API is designed to be a more fuel-efficient regex routine
// which consumes source files into "chunks" that can be properly digested by the
// parser as needed.
//

static inline b32
char_isalpha(char c)
{

    b32 upper = (c >= 0x41 && c <= 0x5A);
    b32 lower = (c >= 0x61 && c <= 0x7A);
    return (upper || lower);

}

static inline b32
char_isnum(char c)
{
    b32 is_number = (c >= 0x30 && c <= 0x39);
    return is_number;
}

static inline b32
char_isalnum(char c)
{
    b32 alpha = char_isalpha(c);
    b32 number = char_isnum(c);
    return (alpha || number);
}

b32     
source_tokenizer_isalpha(source_tokenizer *tokenizer)
{
    
    char current = tokenizer->source[tokenizer->step];
    b32 is_alpha = char_isalpha(current);
    return is_alpha;

}

b32     
source_tokenizer_isnum(source_tokenizer *tokenizer)
{

    char current = tokenizer->source[tokenizer->step];
    b32 is_number = char_isnum(current);
    return is_number;

}

b32     
source_tokenizer_isalnum(source_tokenizer *tokenizer)
{

    char current = tokenizer->source[tokenizer->step];
    b32 is_alnum = char_isalnum(current);
    return current;

}

b32     
source_tokenizer_eof(source_tokenizer *tokenizer)
{

    if (tokenizer->source[tokenizer->step] == '\0')
        return true;
    return false;

}

b32     
source_tokenizer_eol(source_tokenizer *tokenizer)
{
    
    if (source_tokenizer_match(tokenizer, 1, '\n'))
        return true;
    return false;

}

char    
source_tokenizer_peek(source_tokenizer *tokenizer, u64 offset)
{

    char peek = tokenizer->source[tokenizer->step + offset];
    return peek;

}

char    
source_tokenizer_consume(source_tokenizer *tokenizer, u64 count)
{

    char result = tokenizer->source[tokenizer->step + count - 1];
    tokenizer->step += count;
    return result;

}

void    
source_tokenizer_synchronize(source_tokenizer *tokenizer)
{
    
    tokenizer->offset = tokenizer->step;
    return;

}

b32
source_tokenizer_match(source_tokenizer *tokenizer, u32 count, ...)
{

    va_list args;
    va_start(args, count);
    b32 matched = false;

    char current = source_tokenizer_peek(tokenizer, 0);
    for (u32 idx = 0; idx < count; ++idx)
    {
        char argument_character = va_arg(args, char);
        if (argument_character == current)
        {
            matched = true;
            break;
        }
    }

    va_end(args);

    return matched;

}

void
source_tokenizer_check_identifier(source_tokenizer *tokenizer, source_token *token)
{

    // Ensure our list is set.
    static initialized_keywords = false;
    static cc64 keywords[32];
    static source_token_type types[32];
    if (initialized_keywords == false)
    {
        keywords[0]     = "BEGIN";
        types[0]        = TOKEN_KEYWORD_BEGIN;
        keywords[1]     = "ELSEIF";
        types[1]        = TOKEN_KEYWORD_ELSEIF;
        keywords[2]     = "END";
        types[2]        = TOKEN_KEYWORD_END;
        keywords[3]     = "ENDFIT";
        types[3]        = TOKEN_KEYWORD_ENDFIT;
        keywords[4]     = "ENDIF";
        types[4]        = TOKEN_KEYWORD_ENDIF;
        keywords[5]     = "ENDFUNCTION";
        types[5]        = TOKEN_KEYWORD_ENDFUNCTION;
        keywords[6]     = "ENDLOOP";
        types[6]        = TOKEN_KEYWORD_ENDLOOP;
        keywords[7]     = "ENDPLOOP";
        types[7]        = TOKEN_KEYWORD_ENDPLOOP;
        keywords[8]     = "ENDPROCEDURE";
        types[8]        = TOKEN_KEYWORD_ENDPROCEDURE;
        keywords[9]     = "ENDSCOPE";
        types[9]        = TOKEN_KEYWORD_ENDSCOPE;
        keywords[10]    = "ENDWHILE";
        types[10]       = TOKEN_KEYWORD_ENDWHILE;
        keywords[11]    = "FIT";
        types[11]       = TOKEN_KEYWORD_FIT;
        keywords[12]    = "FUNCTION";
        types[12]       = TOKEN_KEYWORD_FUNCTION;
        keywords[13]    = "IF";
        types[13]       = TOKEN_KEYWORD_IF;
        keywords[14]    = "INCLUDE";
        types[14]       = TOKEN_KEYWORD_INCLUDE;
        keywords[15]    = "LOOP";
        types[15]       = TOKEN_KEYWORD_LOOP;
        keywords[16]    = "PLOOP";
        types[16]       = TOKEN_KEYWORD_PLOOP;
        keywords[17]    = "PROCEDURE";
        types[17]       = TOKEN_KEYWORD_PROCEDURE;
        keywords[18]    = "READ";
        types[18]       = TOKEN_KEYWORD_READ;
        keywords[19]    = "SAVE";
        types[19]       = TOKEN_KEYWORD_SAVE;
        keywords[20]    = "SCOPE";
        types[20]       = TOKEN_KEYWORD_SCOPE;
        keywords[21]    = "VARIABLE";
        types[21]       = TOKEN_KEYWORD_VARIABLE;
        keywords[22]    = "WHILE";
        types[22]       = TOKEN_KEYWORD_WHILE;
        keywords[23]    = "WRITE";
        types[23]       = TOKEN_KEYWORD_WRITE;

        initialized_keywords = true;
    }

    // Validate identifier to keywords.
    char hold_character = tokenizer->source[tokenizer->step];
    tokenizer->source[tokenizer->step] = '\0';
    cc64 string = token->source + token->offset;
    u64 length = token->length;

    // NOTE(Chris): We are using a pretty scuffed string matching routine for this.
    //              There are better ways to do this matching, but for now we just
    //              want it to work.
    for (u32 idx = 0; idx < 24; ++idx)
    {
        
        u32 s = 0;
        b32 f = false;
        while(toupper(string[s]) == keywords[idx][s])
        {
            if (string[s] == '\0' && keywords[idx][s] != '\0')
                break;
            if (string[s] != '\0' && keywords[idx][s] == '\0')
                break;
            if (string[s] == '\0' && keywords[idx][s] == '\0')
            {
                f = true;
                break;
            }
            s++;
        }
        if (f == true)
        {
            token->type = types[idx];
            break;
        } 
    }

    // Return hold character.
    tokenizer->source[tokenizer->step] = hold_character;

}

void    
source_tokenizer_set_token(source_tokenizer *tokenizer, source_token *token, source_token_type type)
{

    token->type = type;
    token->source = tokenizer->source;
    token->offset = tokenizer->offset;
    token->length = tokenizer->step - tokenizer->offset;
    return;

}

b32     
source_tokenizer_consume_whitespace(source_tokenizer *tokenizer, source_token *token)
{
    
    if (source_tokenizer_match(tokenizer, 3, '\t', ' ', '\r'))
    {
        source_tokenizer_consume(tokenizer, 1);
        source_tokenizer_synchronize(tokenizer);
        return true;
    }

    return false;

}

b32     
source_tokenizer_match_newline(source_tokenizer *tokenizer, source_token *token)
{

    char current = source_tokenizer_peek(tokenizer, 0);
    if (current == '\n')
    {
        source_tokenizer_consume(tokenizer, 1);
        source_tokenizer_set_token(tokenizer, token, TOKEN_NEW_LINE);
        source_tokenizer_synchronize(tokenizer);
        return true;
    }

    return false;

}

b32     
source_tokenizer_match_comments(source_tokenizer *tokenizer, source_token *token)
{

    char current = source_tokenizer_peek(tokenizer, 0);
    if (current == '{')
    {

        source_tokenizer_consume(tokenizer, 1);
        source_tokenizer_synchronize(tokenizer);

        while (source_tokenizer_peek(tokenizer, 0) != '}' &&
              !source_tokenizer_eof(tokenizer))
        {
            source_tokenizer_consume(tokenizer, 1);
        }

        // The comment could reach EOF, so account for that case and
        // generate the appropriate error token.
        if (source_tokenizer_eof(tokenizer))
        {
            source_tokenizer_set_token(tokenizer, token, TOKEN_UNDEFINED_EOF);
        }
        else
        {

            source_tokenizer_set_token(tokenizer, token, TOKEN_COMMENT_BLOCK);

            // Consume the last '}'.
            source_tokenizer_consume(tokenizer, 1);

        }

        source_tokenizer_synchronize(tokenizer);

        return true;
    }

    return false;

}

b32     
source_tokenizer_match_symbols(source_tokenizer *tokenizer, source_token *token)
{

    char peek = source_tokenizer_peek(tokenizer, 0);
    switch (peek)
    {
        case '(':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_LEFT_PARENTHESIS);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case ')':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_RIGHT_PARENTHESIS);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;
        
        case ';':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_SEMICOLON);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '+':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_PLUS);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '-':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_MINUS);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;
        
        case '*':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_STAR);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '/':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_FORWARD_SLASH);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '^':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_CARROT);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '=':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_EQUALS);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '#':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_HASH);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;
        
        case '&':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_AMPERSAND);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '|':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_PIPE);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '%':
        {
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_PERCENT);
            source_tokenizer_synchronize(tokenizer);
            return true;
        } break;

        case '<':
        {
            char follower = source_tokenizer_peek(tokenizer, 1);

            if (follower == '=')
            {
                source_tokenizer_consume(tokenizer, 2);
                source_tokenizer_set_token(tokenizer, token, TOKEN_LESS_THAN_EQUALS);
                source_tokenizer_synchronize(tokenizer);
                return true;
            }
            
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_LESS_THAN);
            source_tokenizer_synchronize(tokenizer);
            return true;

        } break;

        case '>':
        {
            char follower = source_tokenizer_peek(tokenizer, 1);

            if (follower == '=')
            {
                source_tokenizer_consume(tokenizer, 2);
                source_tokenizer_set_token(tokenizer, token, TOKEN_GREATER_THAN_EQUALS);
                source_tokenizer_synchronize(tokenizer);
                return true;
            }
            
            source_tokenizer_consume(tokenizer, 1);
            source_tokenizer_set_token(tokenizer, token, TOKEN_GREATER_THAN);
            source_tokenizer_synchronize(tokenizer);
            return true;

        } break;

        case ':':
        {
            
            char follower = source_tokenizer_peek(tokenizer, 1);
            if (follower == '=')
            {
                source_tokenizer_consume(tokenizer, 2);
                source_tokenizer_set_token(tokenizer, token, TOKEN_COLON_EQUALS);
                source_tokenizer_synchronize(tokenizer);
                return true;
            }

            return false;

        } break;


    };

    return false;
}

b32     
source_tokenizer_match_numbers(source_tokenizer *tokenizer, source_token *token)
{

    if (source_tokenizer_isnum(tokenizer))
    {

        source_tokenizer_consume(tokenizer, 1);
        
        while (true)
        {

            // Handle decimals.
            char peek = source_tokenizer_peek(tokenizer, 0);
            if (peek == '.')
            {
                char forward = source_tokenizer_peek(tokenizer, 1);
                if (char_isnum(forward))
                {

                    source_tokenizer_consume(tokenizer, 2);
                    continue;

                }
                else
                {
                    break;
                }
            }

            // Consume until non-number.
            if (char_isnum(peek))
            {

                source_tokenizer_consume(tokenizer, 1);
                continue;

            }
            else
            {
                break;
            }

        }

        source_tokenizer_set_token(tokenizer, token, TOKEN_NUMBER);
        source_tokenizer_synchronize(tokenizer);
        return true;

    }

    return false;
}

b32     
source_tokenizer_match_strings(source_tokenizer *tokenizer, source_token *token)
{

    char current = source_tokenizer_peek(tokenizer, 0);
    if (current == '\'')
    {

        source_tokenizer_consume(tokenizer, 1);
        source_tokenizer_synchronize(tokenizer);

        while (source_tokenizer_peek(tokenizer, 0) != '\'' &&
              !source_tokenizer_eof(tokenizer) &&
              !source_tokenizer_eol(tokenizer))
        {
            source_tokenizer_consume(tokenizer, 1);
        }

        // Strings potentially terminate at EOF or EOL, so we check both cases.
        if (source_tokenizer_eof(tokenizer))
        {
            source_tokenizer_set_token(tokenizer, token, TOKEN_UNDEFINED_EOF);
        }

        else if (source_tokenizer_eol(tokenizer))
        {
            source_tokenizer_set_token(tokenizer, token, TOKEN_UNDEFINED_EOL);
        }

        else
        {

            source_tokenizer_set_token(tokenizer, token, TOKEN_STRING);

            // Consume the last '\''.
            source_tokenizer_consume(tokenizer, 1);

        }

        source_tokenizer_synchronize(tokenizer);

        return true;
    }

    return false;
    
}

b32     
source_tokenizer_match_identifiers(source_tokenizer *tokenizer, source_token *token)
{

    if (source_tokenizer_isalpha(tokenizer))
    {

        // Consume until keyword match breaks.
        source_tokenizer_consume(tokenizer, 1);
        while (true)
        {
            char peek = source_tokenizer_peek(tokenizer, 0);
            if (peek == '_' || char_isalnum(peek))
            {
                source_tokenizer_consume(tokenizer, 1);
                continue;
            }
            else
            {
                break;
            }
        }

        source_tokenizer_set_token(tokenizer, token, TOKEN_IDENTIFIER);
        source_tokenizer_synchronize(tokenizer);
        source_tokenizer_check_identifier(tokenizer, token);

        return true;

    }

    return false;
}

void    
source_tokenizer_get_next_token(source_tokenizer *tokenizer, source_token *token)
{

    // Strip all white space before the start of the matching routines.
    while (source_tokenizer_consume_whitespace(tokenizer, token))
    {

    }
    
    // Check if we reached EOF. At EOF and not due to match case EOF, a standard
    // EOF token is generated and returned.
    if (source_tokenizer_eof(tokenizer))
    {
        source_tokenizer_set_token(tokenizer, token, TOKEN_EOF);
        return;
    }

    // Match any cases and return on success.
    if (source_tokenizer_match_newline(tokenizer, token)) return;
    if (source_tokenizer_match_comments(tokenizer, token)) return;
    if (source_tokenizer_match_symbols(tokenizer, token)) return;
    if (source_tokenizer_match_numbers(tokenizer, token)) return;
    if (source_tokenizer_match_strings(tokenizer, token)) return;
    if (source_tokenizer_match_identifiers(tokenizer, token)) return;
    
    // If we didn't return, then we know no cases matches, consume one token and
    // generate an undefined token.
    source_tokenizer_consume(tokenizer, 1);
    source_tokenizer_set_token(tokenizer, token, TOKEN_UNDEFINED);
    source_tokenizer_synchronize(tokenizer);

}

void    
source_tokenizer_initialize(source_tokenizer *tokenizer, c64 source, cc64 path)
{


    tokenizer->file_path = path;
    tokenizer->source = source;
    tokenizer->offset = 0;
    tokenizer->step = 0;

}

// --- Parser ------------------------------------------------------------------
//
// The following code pertains to the parser implementation which generates the
// AST for the language.
//

syntax_node*    
source_parser_create_ast(source_parser *parser, c64 source, cc64 path)
{

    assert(parser != NULL);

    parser->entry           = NULL;
    parser->nodes           = NULL;
    parser->previous_token  = NULL;
    parser->current_token   = NULL;
    parser->next_token      = NULL;

    source_tokenizer_initialize(&parser->tokenizer, source, path);

    syntax_node* root = source_parser_match_expression(parser);
    parser->entry = root;
    parser->nodes = root;
    return root;

}

