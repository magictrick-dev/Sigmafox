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

void    
source_token_position(source_token *token, i32 *line, i32 *col)
{

    assert(token != NULL);
    assert(line != NULL);
    assert(col != NULL);

    i32 line_count = 1;
    i32 column_count = 1;

    u64 offset = 0;
    while (token->source + offset < token->source + token->offset)
    {

        if (token->source[offset] == '\n')
        {
            line_count++;
            column_count = 0;
        }

        offset++;
        column_count++;
    }

    *line = line_count;
    *col = column_count;

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

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

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

        // Determine if the identifier is declared for use in expressions.
        if (!source_parser_identifier_is_declared(parser, object.identifier))
        {
            parser_error_handler_display_error(parser, PARSE_ERROR_UNDEFINED_VARIABLE_IN_EXPRESSION);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        syntax_node *primary_node = source_parser_push_node(parser);
        primary_node->type = PRIMARY_EXPRESSION_NODE;
        primary_node->primary.literal = object;
        primary_node->primary.type = type;

        return primary_node;

    }

    // Groupings.
    else if (source_parser_match_token(parser, 1, TOKEN_LEFT_PARENTHESIS))
    {

        source_parser_consume_token(parser);

        syntax_node *inside = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(inside, parser, mem_state)) return NULL;

        if (!source_parser_expect_token(parser, TOKEN_RIGHT_PARENTHESIS))
        {
            parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_RIGHT_PARENTHESIS);
            return NULL;
        }

        else
        {
            source_parser_consume_token(parser);
        }

        syntax_node *grouping_node = source_parser_push_node(parser);
        grouping_node->type = GROUPING_EXPRESSION_NODE;
        grouping_node->grouping.grouping = inside;

        return grouping_node;


    }
    
    // Error tokens.
    else if (source_parser_match_token(parser, 2, TOKEN_UNDEFINED_EOF, TOKEN_UNDEFINED_EOL))
    {
        parser_error_handler_display_error(parser, PARSE_ERROR_UNEXPECTED_EXPRESSION_EOF_EOL);
        return NULL;
    }

    parser_error_handler_display_error(parser, PARSE_ERROR_UNDEFINED_EXPRESSION_TOKEN);
    return NULL;

}

syntax_node*
source_parser_match_unary(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

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

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *left = source_parser_match_unary(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_STAR, TOKEN_FORWARD_SLASH))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_unary(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

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

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *left = source_parser_match_factor(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_PLUS, TOKEN_MINUS))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_factor(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

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

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *left = source_parser_match_term(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 4, TOKEN_LESS_THAN, TOKEN_LESS_THAN_EQUALS,
            TOKEN_GREATER_THAN, TOKEN_GREATER_THAN_EQUALS))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_term(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

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

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    syntax_node *left = source_parser_match_comparison(parser);
    if (source_parser_should_propagate_error(left, parser, mem_state)) return NULL;

    while (source_parser_match_token(parser, 2, TOKEN_EQUALS, TOKEN_HASH))
    {

        source_token operation = source_parser_consume_token(parser);

        syntax_node *right = source_parser_match_comparison(parser);
        if (source_parser_should_propagate_error(right, parser, mem_state)) return NULL;

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
source_parser_match_expression_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);
    
    syntax_node *expression = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(expression, parser, mem_state))
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // Expect a semi-colon at EOS.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {
        parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_SEMICOLON);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }
    else
    {
        source_parser_consume_token(parser);
        return expression;
    }

}

syntax_node*
source_parser_match_variable_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);
    
    // Consume the VARIABLE token.
    source_parser_consume_token(parser);

    // Generate the variable node.
    syntax_node *variable_node = source_parser_push_node(parser);
    variable_node->type = VARIABLE_STATEMENT_NODE;

    // Check for identifier.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {
        parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_VARIABLE_IDENTIFIER);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    else
    {

        source_token identifier = source_parser_consume_token(parser);

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &identifier, &object);
        assert(type == OBJECT_IDENTIFIER); // This should always be true.

        variable_node->variable.name = object.identifier;

    }

    // The first expression is required.
    syntax_node *size_expression = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(size_expression, parser, mem_state)) 
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    variable_node->variable.size = size_expression;

    // Process all additional expression statement afterwards as array dimensions.
    // Stopping points are :=, ;, or EOF.
    syntax_node *head_dimension_expression = NULL;
    syntax_node *last_dimension_expression = NULL;
    while (!source_parser_match_token(parser, 2, TOKEN_SEMICOLON, TOKEN_COLON_EQUALS))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *expression = source_parser_match_expression(parser);

        // The expression could be NULL, indicating there was an error.
        // Synchronize to the next semi-colon and return an invalid statement.
        if (source_parser_should_propagate_error(expression, parser, mem_state))
        {
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;
        }

        // First expression.
        if (head_dimension_expression == NULL)
        {
            head_dimension_expression = expression;
            last_dimension_expression = expression;
        }

        // All other expressions.
        else
        {
            last_dimension_expression->next_node = expression;   
            last_dimension_expression = expression;
        }
        
    }

    // It is valid for this to evaluate as NULL, optional specification.
    variable_node->variable.dimensions = head_dimension_expression;

    if (source_parser_expect_token(parser, TOKEN_COLON_EQUALS))
    {

        source_parser_consume_token(parser);
        syntax_node *expression = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(expression, parser, mem_state))
        {
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;
        }

        variable_node->variable.assignment = expression;

    }

    // Check for semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {
        parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_SEMICOLON);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        return NULL;
    }
    else
    {
        source_parser_consume_token(parser);
    }

    // Variable is considered valid, we can insert into symbol table.
    symbol *identifier = source_parser_insert_into_symbol_table(parser, variable_node->variable.name);
    if (variable_node->variable.assignment != NULL)
    {
        identifier->type = SYMBOL_TYPE_VARIABLE;
    }
    else
    {
        identifier->type = SYMBOL_TYPE_UNDEFINED;
    }

    return variable_node;

}

syntax_node*
source_parser_match_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);
    syntax_node *result = NULL;

    // Variable statements.
    if (source_parser_expect_token(parser, TOKEN_KEYWORD_VARIABLE))
    {
        result = source_parser_match_variable_statement(parser);
    }

    else
    {
        result = source_parser_match_expression_statement(parser);
    }

    return result;

}

syntax_node*
source_parser_match_program(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Match the begin keyword with trailing semicolon.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_BEGIN))
    {
        parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_PROGRAM_BEGIN);
        return NULL;
    }

    else
    {
        source_parser_consume_token(parser);

        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {
            parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_SEMICOLON);
            return NULL;
        }

        source_parser_consume_token(parser);

    }

    // Generate program syntax node.
    syntax_node *program_node = source_parser_push_node(parser);
    program_node->type = PROGRAM_ROOT_NODE;

    // Match all expression statements and create a statement chain.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_END))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        syntax_node *statement = source_parser_match_statement(parser);

        // The statement could be NULL, which we ignore and move on. Synchronization
        // happens inside statements.
        if (statement == NULL)
        {
            continue;
        }

        // First statement.
        if (head_statement_node == NULL)
        {
            head_statement_node = statement;
            last_statement_node = statement;
        }

        // All other statements.
        else
        {
            last_statement_node->next_node = statement;   
            last_statement_node = statement;
        }

    }

    // Set the program node's next node as the head statement.
    program_node->next_node = head_statement_node;

    // Match the end keyword with trailing semicolon.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_END))
    {
        parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_PROGRAM_END);
        return NULL;
    }
    else
    {

        source_parser_consume_token(parser);

        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {
            parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_SEMICOLON);
            return NULL;
        }

        source_parser_consume_token(parser);

    }

    return program_node;

}

syntax_node*    
source_parser_create_ast(source_parser *parser, cc64 path, memory_arena *arena)
{

    assert(parser != NULL);

    // Cache our memory arena state in the event we need to unwind on error.
    u64 mem_cache = memory_arena_save(arena);

    // Initialize.
    parser->entry           = NULL;
    parser->nodes           = NULL;
    parser->previous_token  = &parser->tokens[0];
    parser->current_token   = &parser->tokens[1];
    parser->next_token      = &parser->tokens[2];
    parser->arena           = arena;
    parser->error_count     = 0;

    // Set the two arenas, the syntax tree arena is a fixed size of 64 megabytes,
    // which equates to (if each syntax node is 48 bytes) to 1.4 million nodes.
    // The transient arena takes the remaining area of the arena and is used for
    // everything else.
    memory_arena_partition(arena, &parser->syntax_tree_arena, SF_MEGABYTES(64));
    u64 primary_arena_remainder_size = memory_arena_free_size(arena);
    memory_arena_partition(arena, &parser->transient_arena, primary_arena_remainder_size);

    // Pull the file source file into memory.
    u64 source_size = fileio_file_size(path);
    char *source_buffer = (char*)memory_arena_push_top(&parser->transient_arena, source_size + 1);
    fileio_file_read(path, source_buffer, source_size, source_size + 1);
    source_buffer[source_size] = '\0'; // Null-terminate.

    // Allocate the global symbol table.
    parser->symbol_table = memory_arena_push_type(&parser->transient_arena, symbol_table);
    symbol_table_initialize(parser->symbol_table, &parser->transient_arena, 1024);

    // Initialize the tokenizer then cycle in two tokens.
    source_tokenizer_initialize(&parser->tokenizer, source_buffer, path);
    source_parser_consume_token(parser);
    source_parser_consume_token(parser);

    // Reserve the string pool.
    string_pool_initialize(&parser->spool, &parser->transient_arena, STRING_POOL_DEFAULT_SIZE);

    // Generate AST from program node.
    syntax_node *program = source_parser_match_program(parser);
    parser->entry = program;
    parser->nodes = program;

    // If there we errors, return NULL and indicate failure to parse.
    if (parser->error_count > 0)
    {
        printf("\n");
        printf("-- Failed to parse, no output has been generated.\n");
        memory_arena_restore(arena, mem_cache);
        return NULL;
    }

    // Pop the transient arena, since we don't need it anymore.
    memory_arena_pop(arena, parser->transient_arena.size);

    return program;

}

syntax_node*
source_parser_push_node(source_parser *parser)
{
    
    syntax_node *allocation = memory_arena_push_type(&parser->syntax_tree_arena, syntax_node);
    allocation->type        = NULL_EXPRESSION_NODE;
    allocation->next_node   = NULL;

    return allocation;

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

    // NOTE(Chris): Due to the fact that expression and statement grammar does
    //              not take into account that new lines and comments can coexist
    //              in them, we ignore them here for now.

    while (parser->next_token->type == TOKEN_NEW_LINE ||
           parser->next_token->type == TOKEN_COMMENT_BLOCK)
    {
        source_tokenizer_get_next_token(&parser->tokenizer, parser->next_token);
    }
    
    return *parser->previous_token;
}

b32 
source_parser_should_break_on_eof(source_parser *parser)
{
    if (source_parser_match_token(parser, 4,
            TOKEN_EOF, 
            TOKEN_UNDEFINED,
            TOKEN_UNDEFINED_EOF,
            TOKEN_UNDEFINED_EOL))
    {
        return true;
    }

    return false;
}

b32 
source_parser_expect_token(source_parser *parser, source_token_type type)
{
    
    b32 is_type = (parser->current_token->type == type);
    return is_type;

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
 
    sh64 pool_entry = string_pool_insert(&parser->spool, string);
    cc64 result = string_pool_string_from_handle(pool_entry);
    return result;

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
source_parser_should_propagate_error(void *check, source_parser *parser, u64 state)
{

    if (check == NULL)
    {
        memory_arena_restore(&parser->syntax_tree_arena, state);
        return true;
    }

    return false;

}

b32
source_parser_synchronize_to(source_parser *parser, source_token_type type)
{

    while (!source_parser_match_token(parser, 2, type, TOKEN_EOF))
    {
        source_parser_consume_token(parser);
    }

    if (source_parser_expect_token(parser, type))
    {
        source_parser_consume_token(parser);
        return true;
    }

    else
    {
        return false;
    }

}

void 
source_parser_push_symbol_table(source_parser *parser)
{

    assert(parser != NULL);
    assert(parser->symbol_table != NULL);

    symbol_table *parent = parser->symbol_table;
    symbol_table *table = memory_arena_push_type(&parser->transient_arena, symbol_table);
    symbol_table_initialize(table, &parser->transient_arena, 1024);

    table->parent = parent;
    parser->symbol_table = table;

}

void 
source_parser_pop_symbol_table(source_parser *parser)
{

    assert(parser != NULL);
    assert(parser->symbol_table != NULL);
    assert(parser->symbol_table->parent != NULL);

    symbol_table *parent_table = parser->symbol_table->parent;
    symbol_table_collapse_arena(parser->symbol_table);
    memory_arena_pop_type(&parser->transient_arena, symbol_table);

}

symbol*
source_parser_insert_into_symbol_table(source_parser *parser, cc64 identifier)
{

    assert(parser != NULL);
    assert(parser->symbol_table != NULL);
    symbol *result = symbol_table_insert(parser->symbol_table, identifier, SYMBOL_TYPE_UNDEFINED);

    r64 current_load_factor = symbol_table_load_factor(parser->symbol_table);
    if (current_load_factor >= 0.66)
    {
        printf("-- Current symbol table reached load factor limit of 0.66, resizing.\n");
        symbol_table_resize(parser->symbol_table);
    }

    return result;


}

b32 
source_parser_identifier_is_declared(source_parser *parser, cc64 identifier)
{

    symbol *result = symbol_table_search_from_any_table(parser->symbol_table, identifier);

    if (result == NULL)
    {
        return false;
    }

    else
    {
        return true;
    }

}

b32 
source_parser_identifier_is_defined(source_parser *parser, cc64 identifier)
{

    symbol *result = symbol_table_search_from_any_table(parser->symbol_table, identifier);

    if (result != NULL && result->type != SYMBOL_TYPE_UNDEFINED)
    {
        return true;
    }

    else
    {
        return false;
    }

}


// --- Error Handling ----------------------------------------------------------
//
// These routines provide verbal error and warnings back to the user in a uniform
// format. Synchronization of the parser should not be performed here.
//

void    
parser_error_handler_display_error(source_parser *parser, parse_error_type error)
{

    parser->error_count++;

    switch (error)
    {
        
        case PARSE_ERROR_UNDEFINED_EXPRESSION_TOKEN:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d): unexpected token in expression: '%s', ",
                    file_name, line, column, error, token_encountered);
            printf("expected real, identifier, or grouping.\n");

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_RIGHT_PARENTHESIS:
        {

            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d): expected matched ')' for expression.\n",
                    file_name, line, column, error, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_PROGRAM_BEGIN:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d): expected program begin statement.\n",
                    file_name, line, column, error, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_PROGRAM_END:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d): expected program end statement.\n",
                    file_name, line, column, error, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_SEMICOLON:
        {

            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d): expected semicolon at end of statement.\n",
                    file_name, line, column, error, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_UNEXPECTED_EXPRESSION_EOF_EOL:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d): unexpected end-of-line or end-of-file.\n",
                    file_name, line, column, error);

            source_token_string_unnullify(error_at, hold);
        } break;

        case PARSE_ERROR_UNDECLARED_IDENTIFIER_IN_EXPRESSION:
        {

            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d): undeclared identifer in expression: '%s'.\n",
                    file_name, line, column, error, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_UNDEFINED_VARIABLE_IN_EXPRESSION:
        {

            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d): undefined variable used in expression: '%s'.\n",
                    file_name, line, column, error, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        default:
        {
            assert(!"Uncaught error message handling routine.");
            return;
        } break;

    };

}

void    
parser_error_handler_display_warning(source_parser *parser, parse_warning_type warning)
{

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

        case PROGRAM_ROOT_NODE:
        {
            
            printf("begin main;\n");
            syntax_node *current_node = root_node->next_node;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("end main;\n");

        } break;

        case VARIABLE_STATEMENT_NODE:
        {
            
            printf("var %s ", root_node->variable.name);
            parser_print_tree(root_node->variable.size);

            syntax_node *current_dim = root_node->variable.dimensions;
            while (current_dim != NULL)
            {
                if (current_dim != NULL) printf(" ");
                printf("[ ");
                parser_print_tree(current_dim);
                printf(" ]");
                current_dim = current_dim->next_node;
            }
            
            if (root_node->variable.assignment != NULL)
            {
                printf(" = ");
                parser_print_tree(root_node->variable.assignment);
            }


        } break;

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
                    printf("%s", root_node->primary.literal.identifier);
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
