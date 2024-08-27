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
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_UNDECLARED_IDENTIFIER_IN_EXPRESSION, __LINE__);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        else if (!source_parser_identifier_is_defined(parser, object.identifier))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_UNDEFINED_IDENTIFIER_IN_EXPRESSION, __LINE__);
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
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_EXPECTED_RIGHT_PARENTHESIS, __LINE__);
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
        parser_error_handler_display_error(parser, PARSE_ERROR_UNEXPECTED_EXPRESSION_EOF_EOL, __LINE__);
        return NULL;
    }

    parser_error_handler_display_error(parser, PARSE_ERROR_UNDEFINED_EXPRESSION_TOKEN, __LINE__);
    return NULL;

}

syntax_node*
source_parser_match_function_call(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    if (source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser,
                parser->current_token, &object);
        cc64 identifier = object.identifier;

        // If it isn't defined, then its probably not a procedure.
        if (!source_parser_identifier_is_defined(parser, identifier))
        {
            syntax_node *forward = source_parser_match_primary(parser);
            return forward;
        }

        // Check if it is procedure call.
        symbol *procedure_call = source_parser_locate_symbol(parser, identifier);
        if (procedure_call->type != SYMBOL_TYPE_FUNCTION)
        {
            syntax_node *forward = source_parser_match_primary(parser);
            return forward;
        }

        // Consume the identifier token.
        source_parser_consume_token(parser);

        // Check for left bracket.
        if (!source_parser_expect_token(parser, TOKEN_LEFT_PARENTHESIS))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_FUNCTION_MISSING_LEFT_PARENTHESIS, __LINE__); 
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // Consume left parenthesis.
        source_parser_consume_token(parser);

        // Now we process expressions until semicolon. Expressions are self-validating
        // and therefore we only need to collect them.
        syntax_node *head_parameter_node = NULL;
        syntax_node *last_parameter_node = NULL;
        i32 arity_count = 0;
        while (!source_parser_match_token(parser, 2, TOKEN_RIGHT_PARENTHESIS, TOKEN_SEMICOLON))
        {

            if (source_parser_should_break_on_eof(parser)) break;
 
            syntax_node *parameter = source_parser_match_expression(parser);
            if (source_parser_should_propagate_error(parameter, parser, mem_state))
            {
                return NULL;
            }

            // Set the parameter.
            if (head_parameter_node == NULL)
            {
                head_parameter_node = parameter;
                last_parameter_node = parameter;
            }

            else
            {
                last_parameter_node->next_node = parameter;
                last_parameter_node = parameter;
            }

            arity_count++;

        }

        // Check for right bracket.
        if (!source_parser_expect_token(parser, TOKEN_RIGHT_PARENTHESIS))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_FUNCTION_MISSING_RIGHT_PARENTHESIS, __LINE__); 
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // Consume right parenthesis.
        source_parser_consume_token(parser);

        syntax_node *call_node = source_parser_push_node(parser);
        call_node->type = FUNCTION_CALL_EXPRESSION_NODE;
        call_node->func_call.identifier = identifier;
        call_node->func_call.parameters = head_parameter_node;
        
        if (arity_count != procedure_call->arity)
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_FUNCTION_ARITY_MISMATCH, __LINE__); 
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // We don't check for semicolons here.
        
        return call_node;

    }

    syntax_node *forward = source_parser_match_primary(parser);
    return forward;

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

    syntax_node *right = source_parser_match_function_call(parser);

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
source_parser_match_procedure_call(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    if (source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {

        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser,
                parser->current_token, &object);
        cc64 identifier = object.identifier;

        // If it isn't defined, then its probably not a procedure.
        if (!source_parser_identifier_is_defined(parser, identifier))
        {
            syntax_node *forward = source_parser_match_expression(parser);
            return forward;
        }

        // Check if it is procedure call.
        symbol *procedure_call = source_parser_locate_symbol(parser, identifier);
        if (procedure_call->type != SYMBOL_TYPE_PROCEDURE)
        {
            syntax_node *forward = source_parser_match_expression(parser);
            return forward;
        }

        // Consume the identifier token.
        source_parser_consume_token(parser);

        // Now we process expressions until semicolon. Expressions are self-validating
        // and therefore we only need to collect them.
        syntax_node *head_parameter_node = NULL;
        syntax_node *last_parameter_node = NULL;
        i32 arity_count = 0;
        while (!source_parser_match_token(parser, 1, TOKEN_SEMICOLON))
        {

            if (source_parser_should_break_on_eof(parser)) break;
 
            syntax_node *parameter = source_parser_match_expression(parser);
            if (source_parser_should_propagate_error(parameter, parser, mem_state))
            {
                source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
                return NULL;
            }

            // Set the parameter.
            if (head_parameter_node == NULL)
            {
                head_parameter_node = parameter;
                last_parameter_node = parameter;
            }

            else
            {
                last_parameter_node->next_node = parameter;
                last_parameter_node = parameter;
            }

            arity_count++;

        }

        syntax_node *call_node = source_parser_push_node(parser);
        call_node->type = PROCEDURE_CALL_EXPRESSION_NODE;
        call_node->proc_call.identifier = identifier;
        call_node->proc_call.parameters = head_parameter_node;
        
        if (arity_count != procedure_call->arity)
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_PROCEDURE_ARITY_MISMATCH, __LINE__); 
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;
        }

        // We don't check for semicolons here.
        
        return call_node;

    }

    syntax_node *forward = source_parser_match_expression(parser);
    return forward;

}

syntax_node*
source_parser_match_assignment(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // An assignment expression begins with an identifier and ':=', so if these
    // two conditions are met, we short circuit the recursive descent and validate
    // that here. Assignment expressions match at equality, ensuring only single-variable
    // assignments are valid.
    b32 current_is_identifier = source_parser_expect_token(parser, TOKEN_IDENTIFIER);
    b32 next_is_assignment = source_parser_next_token_is(parser, TOKEN_COLON_EQUALS);
    if (current_is_identifier && next_is_assignment)
    {

        // Get the object literal.
        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser,
                parser->current_token, &object);
        cc64 identifier = object.identifier;

        // We need to check if the identifier is declared.
        if (!source_parser_identifier_is_declared(parser, identifier))
        {

            parser_error_handler_display_error(parser,
                    PARSE_ERROR_UNDECLARED_VARIABLE_IN_ASSIGNMENT, __LINE__); 
            source_parser_should_propagate_error(NULL, parser, mem_state);
            return NULL;

        }

        // Consume the identifier and assignment operator tokens.
        source_parser_consume_token(parser); // Identifier.
        source_parser_consume_token(parser); // Assignment.

        // Now we can generate the remaining part of the expression.          
        syntax_node *assignment_expression = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(assignment_expression, parser, mem_state))
            return NULL;

        // At this point, the assignment expression should be valid and there
        // is an identifier to set.
        symbol *variable_symbol = source_parser_locate_symbol(parser, identifier);
        if (source_parser_should_propagate_error(variable_symbol, parser, mem_state))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_SYMBOL_UNLOCATABLE, __LINE__);
            return NULL;
        }

        variable_symbol->type = SYMBOL_TYPE_VARIABLE;

        // Now we can return back the syntax node.
        syntax_node *assignment_node = source_parser_push_node(parser);
        assignment_node->type = ASSIGNMENT_EXPRESSION_NODE;
        assignment_node->assignment.identifier = identifier;
        assignment_node->assignment.right = assignment_expression;

        return assignment_node;
        
    }

    syntax_node *forward = source_parser_match_procedure_call(parser);
    return forward;

}

syntax_node*
source_parser_match_expression_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);
    
    syntax_node *expression = source_parser_match_assignment(parser);
    if (source_parser_should_propagate_error(expression, parser, mem_state))
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // Expect a semi-colon at EOS.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
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
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_VARIABLE_IDENTIFIER, __LINE__);
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

        if (source_parser_identifier_is_declared_in_scope(parser, object.identifier))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_VARIABLE_REDECLARATION, __LINE__);
            source_parser_should_propagate_error(NULL, parser, mem_state);
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;
        }

        else if (source_parser_identifier_is_declared_above_scope(parser, object.identifier))
        {
            parser_error_handler_display_warning(parser,
                    PARSE_WARNING_SHADOWED_VARIABLE, __LINE__);
        }

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
        parser_error_handler_display_error(parser, PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        return NULL;
    }
    else
    {
        source_parser_consume_token(parser);
    }

    // Variable is considered valid, we can insert into symbol table.
    symbol *identifier = source_parser_insert_into_symbol_table(parser,
            variable_node->variable.name);
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
source_parser_match_read_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the read token.
    source_parser_consume_token(parser);

    // The following token is an expression which matches the write location.
    syntax_node *location = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(location, parser, mem_state))
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // The following should be a valid, declared identifier.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_IDENTIFIER_IN_READ, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    source_token identifier = source_parser_consume_token(parser);
    object_literal object = {0};
    object_type type = source_parser_token_to_literal(parser, &identifier, &object);
    assert(type == OBJECT_IDENTIFIER); // This should always be true.
    
    if (!source_parser_identifier_is_declared(parser, object.identifier))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_UNDECLARED_VARIABLE_IN_READ, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // If no semicolon, synchronize.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume semiclon.
    source_parser_consume_token(parser);

    syntax_node *read_node = source_parser_push_node(parser);
    read_node->type = READ_STATEMENT_NODE;
    read_node->read.identifier = object.identifier;
    read_node->read.location = location;

    symbol* read_var = source_parser_locate_symbol(parser, object.identifier);
    read_var->type = SYMBOL_TYPE_VARIABLE;

    return read_node;

}

syntax_node*
source_parser_match_write_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the write token.
    source_parser_consume_token(parser);

    // The following token is an expression which matches the write location.
    syntax_node *location = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(location, parser, mem_state))
    {
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // We have the location, now we need the list of statements that follow.
    syntax_node *head_parameter_node = NULL;
    syntax_node *last_parameter_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_SEMICOLON))
    {

        if (source_parser_should_break_on_eof(parser)) break;

        syntax_node *parameter = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(parameter, parser, mem_state))
        {
            source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
            return NULL;
        }

        if (head_parameter_node == NULL)
        {
            head_parameter_node = parameter;
            last_parameter_node = parameter;
        }

        else
        {
            last_parameter_node->next_node = parameter;
            last_parameter_node = parameter;
        }

    }

    // If no semicolon, synchronize.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume semiclon.
    source_parser_consume_token(parser);


    // Create the node.
    syntax_node *write_node = source_parser_push_node(parser);
    write_node->type = WRITE_STATEMENT_NODE;
    write_node->write.location = location;
    write_node->write.body_expressions = head_parameter_node;

    return write_node;

}

syntax_node* 
source_parser_match_scope_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the scope token.
    source_parser_consume_token(parser);

    // If no semicolon, synchronize to the end scope.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        // Synchronize to endscope, then check for semicolon. If it is, consume it
        // so that we align to the next area of parsable code. If not, there is yet
        // another error that needs to be processed.
        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDSCOPE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume semicolon.
    source_parser_consume_token(parser);

    // Create the scope node.
    syntax_node *scope_node = source_parser_push_node(parser);
    scope_node->type = SCOPE_STATEMENT_NODE;

    // Push a new symbol table.
    source_parser_push_symbol_table(parser);

    // Process all statements inside the scope block.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDSCOPE))
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

    // Pop the symbol table.
    source_parser_pop_symbol_table(parser);
    scope_node->scope.body_statements = head_statement_node;

    // We assume now that the ending token is the end scope token.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDSCOPE))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_ENDSCOPE, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDSCOPE);
        return NULL;
    }

    // Consume endscope.
    source_parser_consume_token(parser);

    // If no semicolon, synchronize.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume semiclon.
    source_parser_consume_token(parser);

    return scope_node;

}

syntax_node*
source_parser_match_loop_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the LOOP token.
    source_parser_consume_token(parser);

    // The first parameter of a loop is an identifier.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_IDENTIFIER_IN_LOOP, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;
    }

    // Consume the identifier and then convert it to an object.
    source_token identifier = source_parser_consume_token(parser);
    object_literal object = {0};
    object_type type = source_parser_token_to_literal(parser, &identifier, &object);
    assert(type == OBJECT_IDENTIFIER); // This should always be true.

    // The second part of a loop is the initial bounds.
    syntax_node *initial_bounds = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(initial_bounds, parser, mem_state))
    {

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // The third part of the loop is the exit bounds.
    syntax_node *exit_bounds = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(exit_bounds, parser, mem_state))
    {

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // The fourth part is optional, so only check if the following token
    // isn't the semicolon.
    syntax_node *increment = NULL;
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {
        increment = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(increment, parser, mem_state))
        {

            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;

        }
    }

    // Finally, we require the last token to be a semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Create the while node.
    syntax_node *loop_node = source_parser_push_node(parser);
    loop_node->type = LOOP_STATEMENT_NODE;
    loop_node->for_loop.iterator_identifier = object.identifier;
    loop_node->for_loop.initial_value_expression = initial_bounds;
    loop_node->for_loop.terminate_value_expression = exit_bounds;
    loop_node->for_loop.step_value_expression = increment;

    // Push the iterator symbol into the current table. This iterator is a known
    // variable with a known initial value. This value corresponds to the initial
    // value expression, and each iterator gets reset to the next iterator step value
    // which is by default to be one.
    //
    // NOTE(Chris): We may want to slightly adjust the procedure here to take a
    //              variable node instead, setting it to have an assignment to the
    //              initial value expression. Either way works, but the actual transpilation
    //              will need to take the current format into account that the actual
    //              value of this iterator value is the initial expression + iterator.
    //              Ugly, for sure.
    source_parser_push_symbol_table(parser);
    symbol *iterator_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
    iterator_symbol->type = SYMBOL_TYPE_VARIABLE;

    // Process all statements inside the scope block.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDLOOP))
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

    // Pop the symbol table.
    source_parser_pop_symbol_table(parser);
    loop_node->for_loop.body_statements = head_statement_node;

    // We are assuming the following the token is ENDLOOP token.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDLOOP))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_ENDLOOP, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDLOOP);
        return NULL;
    }

    // Consume ENDLOOP.
    source_parser_consume_token(parser);

    // If no semicolon, synchronize to the next semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume SEMICOLON.
    source_parser_consume_token(parser);

    return loop_node;

}

syntax_node*
source_parser_match_if_statement(source_parser *parser)
{
    
    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the IF token.
    source_parser_consume_token(parser);

    // The first part of the if-statment is a eval-expression, if there's an error,
    // we will probably match until ENDIF to make things easier on the error checker.
    syntax_node *if_expression = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(if_expression, parser, mem_state))
    {

        // Synchronize to the ENDWHILE token.
        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // We have the expression, now we need to terminate the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Generate the syntax node.
    syntax_node *if_node = source_parser_push_node(parser);
    if_node->type = IF_STATEMENT_NODE;
    if_node->if_conditional.evaluation_expression = if_expression;

    // Push a new symbol table.
    source_parser_push_symbol_table(parser);

    // This is the tricky bit here, we need to match to either an ENDIF or an ELSEIF.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 2, TOKEN_KEYWORD_ELSEIF, TOKEN_KEYWORD_ENDIF))
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

    // Pop the symbol table.
    source_parser_pop_symbol_table(parser);
    if_node->if_conditional.body_statements = head_statement_node;

    // Now, we process *all* elseif blocks. It's sort of like we're recursively
    // descending, but this behavior tends to be more self-contained than what is
    // normally done.
    syntax_node *else_head = NULL;
    syntax_node *else_last = NULL;
    while (source_parser_match_token(parser, 1, TOKEN_KEYWORD_ELSEIF))
    {

        // Consume the ELSEIF token or break on EOF.
        source_parser_consume_token(parser);
        if (source_parser_should_break_on_eof(parser)) break; 

        // The following is the elseif eval.
        syntax_node *elseif_expression = source_parser_match_expression(parser);
        if (source_parser_should_propagate_error(elseif_expression, parser, mem_state))
        {

            // Synchronize to the ENDWHILE token.
            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;

        }
        
        // Now we have a semicolon to take care of.
        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {

            parser_error_handler_display_error(parser,
                    PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
            source_parser_should_propagate_error(NULL, parser, mem_state);

            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;

        }

        // Consume the semicolon.
        source_parser_consume_token(parser);

        // Consume all statements until we reach ELSEIF/ENDIF as-per-usual.
        source_parser_push_symbol_table(parser);

        // This is the tricky bit here, we need to match to either an ENDIF or an ELSEIF.
        syntax_node *head_statement_node = NULL;
        syntax_node *last_statement_node = NULL;
        while (!source_parser_match_token(parser, 2, TOKEN_KEYWORD_ELSEIF, TOKEN_KEYWORD_ENDIF))
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

        // Pop the symbol table.
        source_parser_pop_symbol_table(parser);

        // So we have the eval-expression and the body statements, now we need to
        // connect them all together.
        syntax_node *elseif_node = source_parser_push_node(parser);
        elseif_node->type = ELSEIF_STATEMENT_NODE;
        elseif_node->elseif_conditional.body_statements = head_statement_node;
        elseif_node->elseif_conditional.evaluation_expression = elseif_expression;

        if (else_head == NULL)
        {

            // Prepare the chain.
            else_head = elseif_node;
            else_last = elseif_node;

        }
        
        else
        {

            // Chain elseif node.
            else_last->elseif_conditional.else_statement = elseif_node;
            else_last = elseif_node;

        }

    }

    // We need to set the elseif root node.
    if_node->if_conditional.else_statement = else_head;

    // With all that done, we should *expect* the following token to be a ENDIF
    // token or an error, which ever it may be.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDIF))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_ENDIF, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF);
        return NULL;
    }

    // Consume the ENDIF token.
    source_parser_consume_token(parser);

    // Now, at the end of all this jank, we expect the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDIF))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    return if_node;

}

syntax_node* 
source_parser_match_while_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the while token.
    source_parser_consume_token(parser);

    // The following expression is the check expression. If we can't get the expression,
    // we synchronize to the ENDWHILE token which ensures interior statements aren't
    // leaked into further parsing.
    syntax_node *check_expression = source_parser_match_expression(parser);
    if (source_parser_should_propagate_error(check_expression, parser, mem_state))
    {

        // Synchronize to the ENDWHILE token.
        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDWHILE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // If no semicolon, synchronize to the ENDWHILE token.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDWHILE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Create the while node.
    syntax_node *while_node = source_parser_push_node(parser);
    while_node->type = WHILE_STATEMENT_NODE;
    while_node->while_loop.evaluation_expression = check_expression;

    // Push a new symbol table.
    source_parser_push_symbol_table(parser);

    // Process all statements inside the scope block.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDWHILE))
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

    // Pop the symbol table.
    source_parser_pop_symbol_table(parser);
    while_node->while_loop.body_statements = head_statement_node;

    // We are assuming the following the token is ENDWHILE token.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDWHILE))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_ENDWHILE, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDWHILE);
        return NULL;
    }

    // Consume ENDWHILE.
    source_parser_consume_token(parser);

    // If no semicolon, synchronize to the next semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume SEMICOLON.
    source_parser_consume_token(parser);

    return while_node;


}

syntax_node* 
source_parser_match_procedure_statement(source_parser *parser)
{

    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the PROCEDURE token.
    source_parser_consume_token(parser);

    // The first identifier is the name of procedure.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_IDENTIFIER_IN_PROCEDURE, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;
    }

    // Get the identifier.
    source_token identifier = source_parser_consume_token(parser);
    object_literal object = {0};
    object_type type = source_parser_token_to_literal(parser, &identifier, &object);
    assert(type == OBJECT_IDENTIFIER); // This should always be true.

    // We need to check if the identifier for the procedure is already declared.
    if (source_parser_identifier_is_declared(parser, object.identifier))
    {
        
        parser_error_handler_display_error(parser,
                PARSE_ERROR_PROCEDURE_IDENTIFIER_ALREADY_DECLARED, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // The procedure wasn't declared, so we define it here. This goes in the parent
    // scope.
    //
    // NOTE(Chris): Do we want to limit the scope depth of function/procedure decs?
    //              We can probably do this grammatically rather than coding for an
    //              edge case.
    symbol *param_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
    param_symbol->type = SYMBOL_TYPE_PROCEDURE;

    // Push the node.
    syntax_node *procedure_node = source_parser_push_node(parser);
    procedure_node->type = PROCEDURE_STATEMENT_NODE;
    procedure_node->procedure.name = object.identifier;

    // We can push the scope here.
    source_parser_push_symbol_table(parser);

    // Process all identifiers until semicolon, these are the parameters. These
    // parameters are also shoved into the scope for validation.
    syntax_node *head_parameter_node = NULL;
    syntax_node *last_parameter_node = NULL;
    i32 arity_count = 0;
    while (!source_parser_match_token(parser, 1, TOKEN_SEMICOLON))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        
        // Each parameter must be an identifier.
        if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_EXPECTED_IDENTIFIER_IN_PROCEDURE_PARAMS, __LINE__);
            source_parser_should_propagate_error(NULL, parser, mem_state);

            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;
        }

        // Get the identifier.
        source_token identifier = source_parser_consume_token(parser);
        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &identifier, &object);
        assert(type == OBJECT_IDENTIFIER); // This should always be true.

        // Create the parameter node.
        syntax_node *param_node = source_parser_push_node(parser);
        param_node->type = PARAMETER_STATEMENT_NODE;
        param_node->parameter.name = object.identifier;
        param_node->parameter.next_parameter = NULL;

        symbol *param_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
        param_symbol->type = SYMBOL_TYPE_VARIABLE;

        if (head_parameter_node == NULL)
        {
            head_parameter_node = param_node;
            last_parameter_node = param_node;
        }

        else
        {
            last_parameter_node->parameter.next_parameter = param_node;
            last_parameter_node = param_node;
        }

        arity_count++;

    }

    // We should be at the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Set the parameter node in the procedure. This may be NULL, which is fine.
    procedure_node->procedure.parameters = head_parameter_node;
    param_symbol->arity = arity_count;

    // Finally, we process all body statements.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDPROCEDURE))
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


    // Expecting the ENDPROCEDURE.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDPROCEDURE))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_ENDPROCEDURE, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDPROCEDURE);
        return NULL;
    }

    // Consume the ENDIF token.
    source_parser_consume_token(parser);

    // Pop the scope once we're done.
    source_parser_pop_symbol_table(parser);
    procedure_node->procedure.body_statements = head_statement_node;

    // Now, at the end of all this jank, we expect the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    return procedure_node;
}

syntax_node* 
source_parser_match_function_statement(source_parser *parser)
{
 
    u64 mem_state = memory_arena_save(&parser->syntax_tree_arena);

    // Consume the FUNCTION token.
    source_parser_consume_token(parser);

    // The first identifier is the name of procedure.
    if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_IDENTIFIER_IN_PROCEDURE, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;
    }

    // Get the identifier.
    source_token identifier = source_parser_consume_token(parser);
    object_literal object = {0};
    object_type type = source_parser_token_to_literal(parser, &identifier, &object);
    assert(type == OBJECT_IDENTIFIER); // This should always be true.

    // We need to check if the identifier for the procedure is already declared.
    if (source_parser_identifier_is_declared(parser, object.identifier))
    {
        
        parser_error_handler_display_error(parser,
                PARSE_ERROR_FUNCTION_IDENTIFIER_ALREADY_DECLARED, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // The procedure wasn't declared, so we define it here. This goes in the parent
    // scope.
    //
    // NOTE(Chris): Do we want to limit the scope depth of function/procedure decs?
    //              We can probably do this grammatically rather than coding for an
    //              edge case.
    symbol *param_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
    param_symbol->type = SYMBOL_TYPE_FUNCTION;

    // Push the node.
    syntax_node *function_node = source_parser_push_node(parser);
    function_node->type = FUNCTION_STATEMENT_NODE;
    function_node->function.name = object.identifier;

    // We can push the scope here.
    source_parser_push_symbol_table(parser);

    // Push the return symbol onto the stack. When we do this, the variable must evaluate
    // to be defined by the end of the function call, otherwise it is an error.
    symbol *return_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
    return_symbol->type = SYMBOL_TYPE_UNDEFINED;

    // Process all identifiers until semicolon, these are the parameters. These
    // parameters are also shoved into the scope for validation.
    syntax_node *head_parameter_node = NULL;
    syntax_node *last_parameter_node = NULL;
    i32 arity_count = 0;
    while (!source_parser_match_token(parser, 1, TOKEN_SEMICOLON))
    {

        if (source_parser_should_break_on_eof(parser)) break;
        
        // Each parameter must be an identifier.
        if (!source_parser_expect_token(parser, TOKEN_IDENTIFIER))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_EXPECTED_IDENTIFIER_IN_FUNCTION_PARAMS, __LINE__);
            source_parser_should_propagate_error(NULL, parser, mem_state);

            if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION))
            {
                if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                    source_parser_consume_token(parser);
            }

            return NULL;
        }

        // Get the identifier.
        source_token identifier = source_parser_consume_token(parser);
        object_literal object = {0};
        object_type type = source_parser_token_to_literal(parser, &identifier, &object);
        assert(type == OBJECT_IDENTIFIER); // This should always be true.

        // Create the parameter node.
        syntax_node *param_node = source_parser_push_node(parser);
        param_node->type = PARAMETER_STATEMENT_NODE;
        param_node->parameter.name = object.identifier;
        param_node->parameter.next_parameter = NULL;

        symbol *param_symbol = source_parser_insert_into_symbol_table(parser, object.identifier);
        param_symbol->type = SYMBOL_TYPE_VARIABLE;

        if (head_parameter_node == NULL)
        {
            head_parameter_node = param_node;
            last_parameter_node = param_node;
        }

        else
        {
            last_parameter_node->parameter.next_parameter = param_node;
            last_parameter_node = param_node;
        }

        arity_count++;

    }

    // We should be at the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);

        if (source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION))
        {
            if (source_parser_expect_token(parser, TOKEN_SEMICOLON))
                source_parser_consume_token(parser);
        }

        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    // Set the parameter node in the procedure. This may be NULL, which is fine.
    function_node->function.parameters = head_parameter_node;
    param_symbol->arity = arity_count;

    // Finally, we process all body statements.
    syntax_node *head_statement_node = NULL;
    syntax_node *last_statement_node = NULL;
    while (!source_parser_match_token(parser, 1, TOKEN_KEYWORD_ENDFUNCTION))
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

    // Expecting the ENDFUNCTION.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_ENDFUNCTION))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_ENDFUNCTION, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_KEYWORD_ENDFUNCTION);
        return NULL;
    }


    // Consume the ENDFUNCTION.
    source_parser_consume_token(parser);

    // Here's the kicker, we need to now make sure the function's name is considered
    // defined. We can easily check that.
    if (return_symbol->type != SYMBOL_TYPE_VARIABLE)
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_NO_FUNCTION_RETURN_DEFINED, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    // Pop the scope once we're done.
    source_parser_pop_symbol_table(parser);
    function_node->function.body_statements = head_statement_node;

    // Now, at the end of all this jank, we expect the semicolon.
    if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
    {

        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
        source_parser_should_propagate_error(NULL, parser, mem_state);
        source_parser_synchronize_to(parser, TOKEN_SEMICOLON);
        return NULL;

    }

    // Consume the semicolon.
    source_parser_consume_token(parser);

    return function_node;

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

    // Scope statements.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_SCOPE))
    {
        result = source_parser_match_scope_statement(parser);
    }

    // While statements.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_WHILE))
    {
        result = source_parser_match_while_statement(parser);
    }

    // Loop statmeents.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_LOOP))
    {
        result = source_parser_match_loop_statement(parser);
    }

    // If statements.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_IF))
    {
        result = source_parser_match_if_statement(parser);
    }

    // Procedure statement.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_PROCEDURE))
    {
        result = source_parser_match_procedure_statement(parser);
    }
    
    // Function statement.
    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_FUNCTION))
    {
        result = source_parser_match_function_statement(parser);
    }

    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_WRITE))
    {
        result = source_parser_match_write_statement(parser);
    }

    else if (source_parser_expect_token(parser, TOKEN_KEYWORD_READ))
    {
        result = source_parser_match_read_statement(parser);
    }

    // All other expression statements.
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
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_PROGRAM_BEGIN, __LINE__);
        return NULL;
    }

    else
    {
        source_parser_consume_token(parser);

        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
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
    program_node->program.body_statements = head_statement_node;

    // Match the end keyword with trailing semicolon.
    if (!source_parser_expect_token(parser, TOKEN_KEYWORD_END))
    {
        parser_error_handler_display_error(parser,
                PARSE_ERROR_EXPECTED_PROGRAM_END, __LINE__);
        return NULL;
    }
    else
    {

        source_parser_consume_token(parser);

        if (!source_parser_expect_token(parser, TOKEN_SEMICOLON))
        {
            parser_error_handler_display_error(parser,
                    PARSE_ERROR_EXPECTED_SEMICOLON, __LINE__);
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

    // Reserve the string pool and the bottom of the transient arena.
    string_pool_initialize(&parser->spool, &parser->transient_arena, STRING_POOL_DEFAULT_SIZE);

    // Allocate the global symbol table at the bottom of symbol table.
    parser->symbol_table = memory_arena_push_type(&parser->transient_arena, symbol_table);
    symbol_table_initialize(parser->symbol_table, &parser->transient_arena, 1024);

    // Initialize the tokenizer then cycle in two tokens.
    source_tokenizer_initialize(&parser->tokenizer, source_buffer, path);
    source_parser_consume_token(parser);
    source_parser_consume_token(parser);

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
source_parser_next_token_is(source_parser *parser, source_token_type type)
{

    b32 is_type = (parser->next_token->type == type);
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
            cc64 pool_string = source_parser_insert_into_string_pool(parser, token_string);
            object->identifier = pool_string;

            double result = atof(token_string);
            object->real = result;

            source_token_string_unnullify(token, hold);

            return OBJECT_REAL;

        } break;

        case TOKEN_INTEGER:
        {

            char hold;
            cc64 token_string = source_token_string_nullify(token, &hold);
            cc64 pool_string = source_parser_insert_into_string_pool(parser, token_string);
            object->identifier = pool_string;

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

    parser->symbol_table = parent_table;

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
source_parser_identifier_is_declared_in_scope(source_parser *parser, cc64 identifier)
{

    symbol *result = symbol_table_search_from_current_table(parser->symbol_table, identifier);

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
source_parser_identifier_is_declared_above_scope(source_parser *parser, cc64 identifier)
{

    if (parser->symbol_table->parent == NULL) return false;
    symbol *result = symbol_table_search_from_any_table(parser->symbol_table->parent, identifier);

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

symbol* 
source_parser_locate_symbol(source_parser *parser, cc64 identifier)
{

    symbol *result = symbol_table_search_from_any_table(parser->symbol_table, identifier);
    return result;

}



// --- Error Handling ----------------------------------------------------------
//
// These routines provide verbal error and warnings back to the user in a uniform
// format. Synchronization of the parser should not be performed here.
//

void    
parser_error_handler_display_error(source_parser *parser, parse_error_type error, u64 sline)
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

            printf("%s (%d,%d) (error:%d:%llu): unexpected token in expression: '%s', ",
                    file_name, line, column, error, sline, token_encountered);
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

            printf("%s (%d,%d) (error:%d:%llu): expected matched ')' for expression.\n",
                    file_name, line, column, error, sline, token_encountered);

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

            printf("%s (%d,%d) (error:%d:%llu): expected program begin statement.\n",
                    file_name, line, column, error, sline, token_encountered);

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

            printf("%s (%d,%d) (error:%d:%llu): expected program end statement.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_ENDSCOPE:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected endscope keyword.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_NO_FUNCTION_RETURN_DEFINED:
        {

            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): return value of function not defined.\n",
                    file_name, line, column, error, sline);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_ENDWHILE:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected endwhile keyword.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_ENDLOOP:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected endloop keyword.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_ENDPROCEDURE:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected endprocedure keyword.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_ENDFUNCTION:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected endfunction keyword.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_ENDIF:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected endif keyword.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_IDENTIFIER_IN_READ:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected identifier in read statement.\n",
                    file_name, line, column, error, sline);

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

            printf("%s (%d,%d) (error:%d:%llu): expected semicolon at end of statement.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_SYMBOL_UNLOCATABLE:
        {
            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): identifier should be located in "
                   "the symbol table, but was not.\n",
                    file_name, line, column, error, sline);

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

            printf("%s (%d,%d) (error:%d:%llu): unexpected end-of-line or end-of-file.\n",
                    file_name, line, column, error, sline);

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

            printf("%s (%d,%d) (error:%d:%llu): undeclared identifer in expression: '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_UNDEFINED_IDENTIFIER_IN_EXPRESSION:
        {

            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): undefined identifier used in expression: '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_UNDECLARED_VARIABLE_IN_ASSIGNMENT:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): undefined variable used in statement: '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);


        } break;

        case PARSE_ERROR_EXPECTED_IDENTIFIER_IN_LOOP:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected identifier in loop declaration: "
                   " encountered '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_IDENTIFIER_IN_PROCEDURE:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected identifier in procedure declaration: "
                   " encountered '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_IDENTIFIER_IN_PROCEDURE_PARAMS:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected identifier in procedure parameter: "
                   " encountered '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_IDENTIFIER_IN_FUNCTION_PARAMS:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected identifier in function parameter: "
                   " encountered '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_EXPECTED_IDENTIFIER_IN_FUNCTION:
        {

            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected identifier in function declaration:"
                   " encountered '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_PROCEDURE_IDENTIFIER_ALREADY_DECLARED:
        {
            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): procedure name already defined in scope:"
                   " encountered '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);
        } break;

        case PARSE_ERROR_FUNCTION_IDENTIFIER_ALREADY_DECLARED:
        {
            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): function name already defined in scope:"
                   " encountered '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);
        } break;

        case PARSE_ERROR_EXPECTED_ASSIGNMENT:
        {
            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected ':=' in variable "
                   "assignment expression.\n",
                    file_name, line, column, error, sline);

            source_token_string_unnullify(error_at, hold);
        } break;

        case PARSE_ERROR_PROCEDURE_ARITY_MISMATCH:
        {
            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): argument count in procedure call is incorrect.\n",
                    file_name, line, column, error, sline);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_FUNCTION_ARITY_MISMATCH:
        {
            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): argument count in function call is incorrect.\n",
                    file_name, line, column, error, sline);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_FUNCTION_MISSING_LEFT_PARENTHESIS:
        {
            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected '(' in expression for function call.\n",
                    file_name, line, column, error, sline);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_FUNCTION_MISSING_RIGHT_PARENTHESIS:
        {
            source_token *error_at = parser->current_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): expected ')' in expression for function call.\n",
                    file_name, line, column, error, sline);

            source_token_string_unnullify(error_at, hold);

        } break;

        case PARSE_ERROR_VARIABLE_REDECLARATION:
        {
            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): redeclaring a variable that "
                   "is already declared: '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

            source_token_string_unnullify(error_at, hold);
        } break;

        case PARSE_ERROR_UNDECLARED_VARIABLE_IN_READ:
        {
            source_token *error_at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(error_at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(error_at, &hold);

            printf("%s (%d,%d) (error:%d:%llu): identifier in read statement "
                   "is not declared: '%s'.\n",
                    file_name, line, column, error, sline, token_encountered);

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
parser_error_handler_display_warning(source_parser *parser, parse_warning_type warning, u64 sline)
{

    switch (warning)
    {

        case PARSE_WARNING_SHADOWED_VARIABLE:
        {
            source_token *at = parser->previous_token;
            cc64 file_name = parser->tokenizer.file_path;

            i32 line;
            i32 column;

            source_token_position(at, &line, &column);

            char hold;
            cc64 token_encountered = source_token_string_nullify(at, &hold);

            printf("%s (%d,%d) (warning:%d:%llu): "
                   "variable declaration shadows previous declaration: '%s'.\n",
                    file_name, line, column, warning, sline, token_encountered);

            source_token_string_unnullify(at, hold);
        } break;

        default:
        {
            assert(!"Uncaught warning message handling routine.");
            return;
        } break;

    }

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
            syntax_node *current_node = root_node->program.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("end main;\n");

        } break;

        case WRITE_STATEMENT_NODE:
        {

            printf("print ");
            parser_print_tree(root_node->write.location);
            printf(" ");

            syntax_node *current_node = root_node->write.body_expressions;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                current_node = current_node->next_node;
                if (current_node != NULL)
                    printf(" ");
            }


        } break;

        case READ_STATEMENT_NODE:
        {

            printf("read ");
            parser_print_tree(root_node->read.location);
            printf(" %s", root_node->read.identifier);

        } break;

        case SCOPE_STATEMENT_NODE:
        {
            
            printf("{\n");
            syntax_node *current_node = root_node->scope.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case PROCEDURE_CALL_EXPRESSION_NODE:
        {

            printf("%s(", root_node->proc_call.identifier);
            syntax_node *params = root_node->proc_call.parameters;
            while(params != NULL)
            {
                parser_print_tree(params);
                if (params->next_node != NULL) printf(", ");
                params = params->next_node;
            }
            printf(")");

        } break;

        case FUNCTION_CALL_EXPRESSION_NODE:
        {

            printf("f:%s(", root_node->proc_call.identifier);
            syntax_node *params = root_node->proc_call.parameters;
            while(params != NULL)
            {
                parser_print_tree(params);
                if (params->next_node != NULL) printf(", ");
                params = params->next_node;
            }
            printf(")");

        } break;

        case PARAMETER_STATEMENT_NODE:
        {
            printf("%s", root_node->parameter.name);
        } break;

        case FUNCTION_STATEMENT_NODE:
        {

            printf("function %s ( ", root_node->function.name);
            syntax_node *params = root_node->function.parameters;
            while (params != NULL)
            {
                parser_print_tree(params);
                if (params->parameter.next_parameter != NULL) printf(", ");
                params = params->parameter.next_parameter;
            }

            printf(" )\n");
            printf("{\n");
            syntax_node *current_node = root_node->procedure.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case PROCEDURE_STATEMENT_NODE:
        {

            printf("procedure %s ( ", root_node->procedure.name);
            syntax_node *params = root_node->procedure.parameters;
            while (params != NULL)
            {
                parser_print_tree(params);
                if (params->parameter.next_parameter != NULL) printf(", ");
                params = params->parameter.next_parameter;
            }

            printf(" )\n");
            printf("{\n");
            syntax_node *current_node = root_node->procedure.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case ELSEIF_STATEMENT_NODE:
        {
            printf("elseif: ");
            parser_print_tree(root_node->elseif_conditional.evaluation_expression);
            printf("\n");
            printf("{\n");
            syntax_node *current_node = root_node->elseif_conditional.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");
        } break;

        case IF_STATEMENT_NODE:
        {

            printf("if: ");
            parser_print_tree(root_node->if_conditional.evaluation_expression);
            printf("\n");
            printf("{\n");
            syntax_node *current_node = root_node->if_conditional.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

            syntax_node *else_node = root_node->if_conditional.else_statement;
            while (else_node != NULL)
            {
                parser_print_tree(else_node);
                else_node = else_node->elseif_conditional.else_statement;
            }

        } break;

        case WHILE_STATEMENT_NODE:
        {
            
            printf("while: ");
            parser_print_tree(root_node->while_loop.evaluation_expression);
            printf("\n");
            printf("{\n");
            syntax_node *current_node = root_node->while_loop.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

        } break;

        case LOOP_STATEMENT_NODE:
        {
            printf("for %s = ", root_node->for_loop.iterator_identifier);
            parser_print_tree(root_node->for_loop.initial_value_expression);
            printf("; %s < ", root_node->for_loop.iterator_identifier);
            parser_print_tree(root_node->for_loop.terminate_value_expression);
            printf("; %s += ", root_node->for_loop.iterator_identifier);
            if (root_node->for_loop.step_value_expression == NULL)
                printf("1\n");
            else
            {
                parser_print_tree(root_node->for_loop.step_value_expression);
                printf("\n");
            }

            printf("{\n");
            syntax_node *current_node = root_node->for_loop.body_statements;
            while (current_node != NULL)
            {
                parser_print_tree(current_node);
                printf(";\n");
                current_node = current_node->next_node;
            }
            printf("}");

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

        case ASSIGNMENT_EXPRESSION_NODE:
        {
            
            printf("%s = ", root_node->assignment.identifier);
            parser_print_tree(root_node->assignment.right);

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
                case OPERATION_ASSIGNMENT: printf(" = "); break;
                
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
