#include <compiler/rparser.h>

// --- Tokenizer ---------------------------------------------------------------
//
// The tokenizer is responsible for converting a raw-text source file into tokens.
// The majority of this API is designed to be a more fuel-efficient regex routine
// which consumes source files into "chunks" that can be properly digested by the
// parser as needed.
//

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
    
    if (source_tokenizer_match(tokenizer, 2, '\r', '\n'))
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



    return false;
}

b32     
source_tokenizer_match_strings(source_tokenizer *tokenizer, source_token *token)
{

    return false;
}

b32     
source_tokenizer_match_identifiers(source_tokenizer *tokenizer, source_token *token)
{

    return false;
}

void    
source_tokenizer_get_next_token(source_tokenizer *tokenizer, source_token *token)
{

    // Strip all white space before the start of the matching routines.
    source_tokenizer_consume_whitespace(tokenizer, token);
    
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
    
    // If we didn't return, then we know no cases matches, consume one token and
    // generate an undefined token.
    source_tokenizer_consume(tokenizer, 1);
    source_tokenizer_set_token(tokenizer, token, TOKEN_UNDEFINED);
    source_tokenizer_synchronize(tokenizer);

}

void    
source_tokenizer_initialize(source_tokenizer *tokenizer, cc64 source, cc64 path)
{


    tokenizer->file_path = path;
    tokenizer->source = source;
    tokenizer->offset = 0;
    tokenizer->step = 0;

}

