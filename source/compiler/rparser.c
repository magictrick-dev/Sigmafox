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
        source_token_type type = TOKEN_INTEGER;
        
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
                    type = TOKEN_REAL;
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

        source_tokenizer_set_token(tokenizer, token, type);
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
    while (source_tokenizer_consume_whitespace(tokenizer, token));
    
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

cc64    
source_token_string_nullify(source_token *token, char *hold_character)
{

    assert(hold_character != NULL);
    cc64 string_start = token->source + token->offset;

    char hold = token->source[token->offset + token->length];
    *hold_character = hold;

    token->source[token->offset + token->length] = '\0';

    return string_start;

}

void    
source_token_string_unnullify(source_token *token, char hold_character)
{

    token->source[token->offset + token->length] = hold_character;
    return;

}

// --- Parser ------------------------------------------------------------------
//
// The following code pertains to the parser implementation which generates the
// AST for the language.
//
// NOTE(Chris): For anyone who intends to maintain this code, please refer to
//              the grammar specification for a better understanding of how these
//              functions traverse during runtime. The recursive nature makes it
//              difficult to follow by looking at the code alone.
//
//            - Errors are propagated upwards as NULL and should end at an appropriate
//              "synchronization point" where the parser can recover and replace
//              itself to a point where it can continue to process more errors.
//              This is pretty typical behavior of most compilers. At no point
//              should the tree actually contain NULLs. Either the parser fully
//              processes and validates completely or the AST is NULL due to an
//              error. Any generated tree should be fully valid (no NULLs).
//
//            - The memory arena should also restore itself as errors propagate
//              upwards. This unwinding ensures that the final output list of nodes
//              of the generated tree is completely valid regardless if the final
//              AST is invalid and NULL. The current implementation fully restores
//              the arena on error, but we may want to create a debugger in order
//              to track tricky errors that would be otherwise be hard to follow.
//
//            - The parser uses a fixed memory pool and treats out-of-memory
//              conditions as "hard errors" and should force exit the parse routine
//              and display an extremely helpful error message to inform the user
//              what occured and how to adjust the memory parameters to avoid this
//              problem during compilation. It's important that this clear without
//              ambiguity. We should target the compiler to work with low memory
//              overhead, so we should target a maximum of 4GB under extreme conditions
//              and 2GB for normal conditions. If we need more than that, it's a
//              symptom of poor architecture *or* the generated program is fairly
//              substantial. Generally speaking, the latter of which is unlikely.
//

syntax_node*
source_parser_match_primary(source_parser *parser)
{

    // Literals.
    if (source_parser_match_token(parser, 3, TOKEN_REAL, TOKEN_INTEGER, TOKEN_STRING))
    {

        source_token literal = source_parser_consume_token(parser);

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &literal, &object);

        syntax_node *primary_node = source_parser_push_node(parser);
        primary_node->type = PRIMARY_EXPRESSION_NODE;
        primary_node->primary.literal = object;
        primary_node->primary.type = type;
        return primary_node;

    }

    // Identifiers.
    else if (source_parser_match_token(parser, 1, TOKEN_IDENTIFIER))
    {
        
        source_token identifier = source_parser_consume_token(parser);

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &identifier, &object);

        syntax_node *primary_node = source_parser_push_node(parser);
        primary_node->type = PRIMARY_EXPRESSION_NODE;
        primary_node->primary.literal = object;
        primary_node->primary.type = type;

        return primary_node;

    }

    return NULL;

}

syntax_node*
source_parser_match_unary(source_parser *parser)
{

    arena_state mem_state = memory_arena_cache_state(parser->arena);

    if (source_parser_match_token(parser, 1, TOKEN_MINUS))
    {
        
        syntax_operation_type operation = OPERATION_NEGATIVE_ASSOCIATE;
        source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_unary(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

        syntax_node *unary_node = source_parser_push_node(parser);
        unary_node->type = UNARY_EXPRESSION_NODE;
        unary_node->unary.right = right;
        unary_node->unary.type = operation;

        return unary_node;

    }

    syntax_node *right = source_parser_match_primary(parser);

    return right;

}

syntax_node*
source_parser_match_factor(source_parser *parser)
{

    arena_state mem_state = memory_arena_cache_state(parser->arena);

    syntax_node *left = source_parser_match_unary(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_STAR, TOKEN_FORWARD_SLASH))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_unary(parser);
        if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

        syntax_node *binary_node = source_parser_push_node(parser);
        binary_node->type = BINARY_EXPRESSION_NODE;
        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.type = source_parser_token_to_operation(&operation);
        
        left = binary_node;

    }

    return left;

}

syntax_node*
source_parser_match_term(source_parser *parser)
{

    arena_state mem_state = memory_arena_cache_state(parser->arena);

    syntax_node *left = source_parser_match_factor(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_PLUS, TOKEN_MINUS))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_factor(parser);
        if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

        syntax_node *binary_node = source_parser_push_node(parser);
        binary_node->type = BINARY_EXPRESSION_NODE;
        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.type = source_parser_token_to_operation(&operation);
        
        left = binary_node;

    }

    return left;

}

syntax_node*
source_parser_match_comparison(source_parser *parser)
{

    arena_state mem_state = memory_arena_cache_state(parser->arena);

    syntax_node *left = source_parser_match_term(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 4, TOKEN_LESS_THAN, TOKEN_LESS_THAN_EQUALS,
            TOKEN_GREATER_THAN, TOKEN_GREATER_THAN_EQUALS))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_term(parser);
        if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

        syntax_node *binary_node = source_parser_push_node(parser);
        binary_node->type = BINARY_EXPRESSION_NODE;
        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.type = source_parser_token_to_operation(&operation);
        
        left = binary_node;

    }

    return left;

}

syntax_node*
source_parser_match_equality(source_parser *parser)
{

    arena_state mem_state = memory_arena_cache_state(parser->arena);

    syntax_node *left = source_parser_match_comparison(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_EQUALS, TOKEN_HASH))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_comparison(parser);
        if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

        syntax_node *binary_node = source_parser_push_node(parser);
        binary_node->type = BINARY_EXPRESSION_NODE;
        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.type = source_parser_token_to_operation(&operation);

        left = binary_node;

    }

    return left;

}

syntax_node*
source_parser_match_expression(source_parser *parser)
{

    syntax_node *expression = source_parser_match_equality(parser);
    return expression;

}

syntax_node*
source_parser_push_node(source_parser *parser)
{
    
    syntax_node *allocation = memory_arena_push_type(parser->arena, syntax_node);
    return allocation;

}

syntax_node*    
source_parser_create_ast(source_parser *parser, c64 source, cc64 path, memory_arena *arena)
{

    assert(parser != NULL);

    // Cache our memory arena state in the event we need to unwind on error.
    arena_state mem_cache = memory_arena_cache_state(arena);

    // Initialize.
    parser->entry           = NULL;
    parser->nodes           = NULL;
    parser->previous_token  = &parser->tokens[0];
    parser->current_token   = &parser->tokens[1];
    parser->next_token      = &parser->tokens[2];
    parser->arena           = arena;

    // Get our current token and then get the "peek" token.
    source_tokenizer_initialize(&parser->tokenizer, source, path);
    source_tokenizer_get_next_token(&parser->tokenizer, parser->current_token);
    source_tokenizer_get_next_token(&parser->tokenizer, parser->next_token);

    // Reserve the string pool.
    c64 string_pool_buffer = memory_arena_push_array(arena, char, STRING_POOL_DEFAULT_SIZE);
    parser->string_pool_buffer = string_pool_buffer;
    parser->string_pool_size = STRING_POOL_DEFAULT_SIZE;

    // Generate the tree.
    syntax_node* root = source_parser_match_expression(parser);
    parser->entry = root;
    parser->nodes = root;
    
    // If there was an error, we can just restore our last mem-cache point.
    if (root == NULL)
    {
        printf("Failed to parse.\n");
        memory_arena_restore_state(arena, mem_cache);
    }

    return root;

}

source_token
source_parser_get_previous_token(source_parser *parser)
{
    source_token *result = parser->previous_token;
    return *result;
}

source_token
source_parser_get_current_token(source_parser *parser)
{
    source_token *result = parser->current_token;
    return *result;
}

source_token
source_parser_get_next_token(source_parser *parser)
{
    source_token *result = parser->next_token;
    return *result;
}

source_token
source_parser_consume_token(source_parser *parser)
{
    source_token *temporary = parser->previous_token;
    parser->previous_token = parser->current_token;
    parser->current_token = parser->next_token;
    parser->next_token = temporary;
    source_tokenizer_get_next_token(&parser->tokenizer, parser->next_token);
    return *parser->previous_token;
}

b32 
source_parser_match_token(source_parser *parser, u32 count, ...)
{

    va_list args;
    va_start(args, count);
    b32 matched = false;

    source_token_type current = parser->current_token->type;
    for (u32 idx = 0; idx < count; ++idx)
    {
        source_token_type type = va_arg(args, source_token_type);
        if (type == current)
        {
            matched = true;
            break;
        }
    }

    va_end(args);

    return matched;

}

object_type
source_parser_token_to_literal(source_parser *parser, source_token *token, object_literal *object)
{
    
    source_token_type type = token->type;

    switch(type)
    {

        case TOKEN_REAL:
        {

            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);

            double result = atof(token_string);
            object->real = result;

            source_token_string_unnullify(token, hold);

            return OBJECT_REAL;

        } break;

        case TOKEN_INTEGER:
        {

            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);

            i64 result = atoll(token_string);
            object->signed_integer = result;

            source_token_string_unnullify(token, hold);
            return OBJECT_SIGNED_INTEGER;
            
        } break;

        case TOKEN_IDENTIFIER:
        {

            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);

            cc64 pool_string = source_parser_insert_into_string_pool(parser, token_string);
            object->string = pool_string;

            source_token_string_unnullify(token, hold);
            return OBJECT_IDENTIFIER;

        } break;

        case TOKEN_STRING:
        {
            
            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);

            cc64 pool_string = source_parser_insert_into_string_pool(parser, token_string);
            object->string = pool_string;

            source_token_string_unnullify(token, hold);
            return OBJECT_STRING;

        } break;

    };

    assert(!"Unreachable condition, not all types handled.");
    return 0; // So the compiler doesn't complain.

}

cc64 
source_parser_insert_into_string_pool(source_parser *parser, cc64 string)
{
 
    c64 current = parser->string_pool_buffer + parser->string_pool_offset;
    u64 string_index = 0;
    u64 string_length = strlen(string);
    
    // TODO(Chris): We should probably clean this up for error handling in the
    //              future since we need to know when the pool buffer reaches
    //              capacity. We also don't want to leave the pool buffer as a
    //              trivial subroutine since this is a critical point of optimization.
    assert(string_length + parser->string_pool_offset <= parser->string_pool_size);

    while (true)
    {
        c64 i = parser->string_pool_buffer + parser->string_pool_offset + string_index;
        *i = string[string_index];
        if (string_index == '\0')
        {
            string_index++;
            break;
        }
        else
        {
            string_index++;
        }
    }

    parser->string_pool_offset = string_index;

    return current;

}

syntax_operation_type 
source_parser_token_to_operation(source_token *token)
{

    source_token_type type = token->type;

    switch (type)
    {
        case TOKEN_PLUS:                return OPERATION_ADDITION;
        case TOKEN_MINUS:               return OPERATION_SUBTRACTION;
        case TOKEN_STAR:                return OPERATION_MULTIPLICATION;
        case TOKEN_FORWARD_SLASH:       return OPERATION_DIVISION;
        case TOKEN_EQUALS:              return OPERATION_EQUALS;
        case TOKEN_HASH:                return OPERATION_NOT_EQUALS;
        case TOKEN_LESS_THAN:           return OPERATION_LESS_THAN;
        case TOKEN_LESS_THAN_EQUALS:    return OPERATION_LESS_THAN_EQUALS;
        case TOKEN_GREATER_THAN:        return OPERATION_GREATER_THAN;
        case TOKEN_GREATER_THAN_EQUALS: return OPERATION_GREATER_THAN_EQUALS;
    };

    assert(!"Unreachable condition, not all types handled.");
    return 0; // So the compiler doesn't complain.

}

b32 
source_parser_should_propagate_error(void *check, source_parser *parser, arena_state state)
{

    if (check == NULL)
    {
        memory_arena_restore_state(parser->arena, state);
        return true;
    }

    return false;

}


// --- Print Traversal ---------------------------------------------------------
//
// The following print traversal is designed for viewing the raw output of the
// tree's interpretation. It's mainly used for debugging and not meant to be
// used as production code.
//

void
parser_print_tree(syntax_node *root_node)
{

    switch(root_node->type)
    {

        case BINARY_EXPRESSION_NODE:
        {

            parser_print_tree(root_node->binary.left);

            switch (root_node->binary.type)
            {

                case OPERATION_ADDITION: printf(" + "); break;
                case OPERATION_SUBTRACTION: printf(" - "); break;
                case OPERATION_MULTIPLICATION: printf(" * "); break;
                case OPERATION_DIVISION: printf(" / "); break;
                case OPERATION_EQUALS: printf(" == "); break;
                case OPERATION_NOT_EQUALS: printf(" != "); break;
                case OPERATION_LESS_THAN: printf(" < "); break;
                case OPERATION_LESS_THAN_EQUALS: printf(" <= "); break;
                case OPERATION_GREATER_THAN: printf(" > "); break;
                case OPERATION_GREATER_THAN_EQUALS: printf(" >= "); break;
                
                default:
                {

                    assert(!"Unimplemented operation for binary expression printing.");
                    return;

                } break;
            }

            parser_print_tree(root_node->binary.right);

        } break;

        case UNARY_EXPRESSION_NODE:
        {

            switch (root_node->unary.type)
            {

                case OPERATION_NEGATIVE_ASSOCIATE: printf("-"); break;

                default:
                {

                    assert(!"Unimplemented operation for unary expression printing.");
                    return;

                } break;

            };

            parser_print_tree(root_node->unary.right);

        } break;

        case GROUPING_EXPRESSION_NODE:
        {

            printf("( ");
            parser_print_tree(root_node->grouping.grouping);
            printf(" )");

        } break;

        case PRIMARY_EXPRESSION_NODE:
        {

            switch (root_node->primary.type)
            {

                case OBJECT_UNSIGNED_INTEGER: 
                {
                    printf("%llu", root_node->primary.literal.unsigned_integer);
                } break;

                case OBJECT_SIGNED_INTEGER: 
                {
                    printf("%lld", root_node->primary.literal.signed_integer);
                } break;

                case OBJECT_REAL: 
                {
                    printf("%f", root_node->primary.literal.real);
                } break;

                case OBJECT_BOOLEAN: 
                {
                    printf("%lld", root_node->primary.literal.boolean);
                } break;

                case OBJECT_STRING: 
                {
                    printf("\"%s\"", root_node->primary.literal.string);
                } break;

                case OBJECT_IDENTIFIER: 
                {
                    printf("\"%s\"", root_node->primary.literal.identifier);
                } break;
                
                default:
                {

                    assert(!"Unimplemented case for primary expression printing.");
                    return;

                } break;

            }

        } break;

        default:
        {

            assert(!"Unimplemented parser print case.");
            return;

        };

    };

}
