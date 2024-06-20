#include <cstdio>
#include <compiler/parser.h>

#define propagate_on_error(exp) if ((exp) == NULL) return NULL

static inline void
parser_display_error(token *location, const char *reason)
{
    
    printf("%s(%lld,%lld): %s\n", location->location, location->line,
            location->offset_from_start - location->offset_from_line + 1, reason);

}

// --- Token Helper Functions --------------------------------------------------
//
// A set of helper functions regarding tokens.
//

string  
parser_token_to_string(token *instance)
{
    string result(instance->length + 1);
    for (size_t idx = 0; idx < instance->length; ++idx)
        result[idx] = instance->source[instance->offset_from_start + idx];
    return result;
}

string  
parser_token_type_to_string(token *instance)
{

    switch (instance->type)
    {

        case token_type::COMMENT_BLOCK: return "comment_block";          
        case token_type::LEFT_PARENTHESIS: return "left_parenthesis";      
        case token_type::RIGHT_PARENTHESIS: return "right_parenthesis";    
        case token_type::SEMICOLON: return "semicolon";           
        case token_type::ASSIGNMENT: return "assignment";         
        case token_type::PLUS: return "plus";              
        case token_type::MINUS: return "minus";            
        case token_type::MULTIPLY: return "multiply";        
        case token_type::DIVISION: return "division";       
        case token_type::POWER: return "power";         
        case token_type::LESS_THAN: return "less_than";             
        case token_type::LESS_THAN_EQUALS: return "less_than_equals";     
        case token_type::GREATER_THAN: return "greater_than";        
        case token_type::GREATER_THAN_EQUALS: return "greater_than_equals"; 
        case token_type::EQUALS: return "equals";             
        case token_type::NOT_EQUALS: return "not_equals";        
        case token_type::CONCAT: return "concat";           
        case token_type::EXTRACT: return "extract";         
        case token_type::DERIVATION: return "derivation";     
        case token_type::IDENTIFIER: return "identifier";
        case token_type::STRING: return "string";   
        case token_type::NUMBER: return "number";
        case token_type::BEGIN: return "begin";      
        case token_type::END: return "end";
        case token_type::PROCEDURE: return "procedure";  
        case token_type::ENDPROCEDURE: return "endprocedure";
        case token_type::FUNCTION: return "function";   
        case token_type::ENDFUNCTION: return "endfunction";
        case token_type::IF: return "if";         
        case token_type::ENDIF: return "endif";
        case token_type::WHILE: return "while";      
        case token_type::ENDWHILE: return "endwhile";
        case token_type::LOOP: return "loop";       
        case token_type::ENDLOOP: return "endloop";
        case token_type::PLOOP: return "ploop";      
        case token_type::ENDPLOOP: return "endploop";
        case token_type::FIT: return "fit";        
        case token_type::ENDFIT: return "endfit";
        case token_type::VARIABLE: return "variable";
        case token_type::WRITE: return "write";
        case token_type::READ: return "read";
        case token_type::SAVE: return "save";
        case token_type::INCLUDE: return "include";
        case token_type::UNDEFINED: return "undefined";
        case token_type::END_OF_FILE: return "end_of_file";
        case token_type::END_OF_LINE: return "end_of_line";
        default: return "unset conversion";

    }

}


// --- Source File Scanner -----------------------------------------------------
//
// Sequentially scans a source file by inspecting leading characters and properly
// matching expressions into tokens.
//

struct scanner
{
    const char *source;
    const char *file;
    size_t step;
    size_t start;
    size_t line;
    size_t line_offset;
};

static inline bool
scanner_is_eof(scanner *state)
{
    bool eof_marker =  (state->source[state->step] == '\0'   ||
                        state->source[state->step] == '\13'  ||
                        state->source[state->step] == '\10');
    return eof_marker;
}

static inline bool
scanner_is_line_control(scanner *state)
{
    bool line_marker = (state->source[state->step] == '\r' || 
                        state->source[state->step] == '\n');
    return line_marker;
}

static inline char
scanner_advance(scanner *state)
{
    char current = state->source[state->step];
    state->step++;
    return current;
}

static inline char
scanner_peek(scanner *state)
{
    char current = state->source[state->step];
    return current;
}

static inline token_type
scanner_validate_identifier_type(token *current_token)
{
    
    string current = parser_token_to_string(current_token);
    current.to_uppercase();
    
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
        keyword_list[9] = "FIT";
        type_list[9] = token_type::FIT;
        keyword_list[10] = "FUNCTION";
        type_list[10] = token_type::FUNCTION;
        keyword_list[11] = "IF";
        type_list[11] = token_type::IF;
        keyword_list[12] = "INCLUDE";
        type_list[12] = token_type::INCLUDE;
        keyword_list[13] = "LOOP";
        type_list[13] = token_type::LOOP;
        keyword_list[14] = "PLOOP";
        type_list[14] = token_type::PLOOP;
        keyword_list[15] = "PROCEDURE";
        type_list[15] = token_type::PROCEDURE;
        keyword_list[16] = "READ";
        type_list[16] = token_type::READ;
        keyword_list[17] = "SAVE";
        type_list[17] = token_type::SAVE;
        keyword_list[18] = "VARIABLE";
        type_list[18] = token_type::VARIABLE;
        keyword_list[19] = "WHILE";
        type_list[19] = token_type::WHILE;
        keyword_list[20] = "WRITE";
        type_list[20] = token_type::WRITE;

        list_initialized = true;
    }

    // NOTE(Chris): String compares are somewhat expensive, so the trick is to
    //              reduce the amount of compares by alphabetizing into buckets
    //              and searching that way.
    switch (current[0])
    {

        case 'B':
        {
            if (keyword_list[0] == current)
                return type_list[0];
        } break;

        case 'E':
        {
            for (size_t idx = 1; idx < 9; ++idx)
            {
                if (keyword_list[idx] == current)
                    return type_list[idx];
            }
        } break;

        case 'F':
        {
            for (size_t idx = 9; idx < 11; ++idx)
            {
                if (keyword_list[idx] == current)
                    return type_list[idx];
            }
        } break;

        case 'I':
        {
            for (size_t idx = 11; idx < 13; ++idx)
            {
                if (keyword_list[idx] == current)
                    return type_list[idx];
            }
        } break;
    
        case 'L':
        {
            if (keyword_list[13] == current)
                return type_list[13];
        } break;

        case 'P':
        {
            for (size_t idx = 14; idx < 16; ++idx)
            {
                if (keyword_list[idx] == current)
                    return type_list[idx];
            }
        } break;

        case 'R':
        {
            if (keyword_list[16] == current)
                return type_list[16];
        } break;

        case 'S':
        {
            if (keyword_list[17] == current)
                return type_list[17];
        } break;

        case 'V':
        {
            if (keyword_list[18] == current)
                return type_list[18];
        } break;
            
        case 'W':
        {
            for (size_t idx = 19; idx < 21; ++idx)
            {
                if (keyword_list[idx] == current)
                    return type_list[idx];
            }
        } break;
    };

    return current_token->type;

}


static inline token
scanner_create_token(scanner *state, token_type type)
{

    token result = {};
    result.source               = state->source;
    result.location             = state->file;
    result.offset_from_start    = state->start;
    result.offset_from_line     = state->line_offset;
    result.length               = state->step - state->start;
    result.line                 = state->line;
    result.type                 = type;

    if (type == token_type::IDENTIFIER)
        result.type = scanner_validate_identifier_type(&result);

    return result;

}

bool 
parser_tokenize_source_file(const char *source_name, const char *source_file, 
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

// --- Abstract Syntax Tree ----------------------------------------------------
//
// An abstract syntax tree takes the set of linear tokens and composes them into
// a tree that corresponds to the language grammar. The basis for the language is
// expressions. We use recursive descent to the generate the required grammar.
//
// Expression Grammar
//      - Defines expressions through recursive descent which enforces order
//        of operations. The complete grammar references this for statements.
//
// expression   : equality
// equality     : comparison ( ( "=" | "#" ) comparison )*
// comparison   : term ( ( "<" | ">" | "<=" | ">=" ) term )*
// term         : factor ( ( "+" | "-" ) factor )*
// factor       : unary ( ( "*" | "/" ) unary )*
// unary        : ( "-" ) unary | primary
// primary      : NUMBER | STRING | "true" | "false" | "(" expression ")"
//
// Complete Language Grammar
//
// program              : statement* EOF
// statement            : declaration_stm | expression_stm
// declaration_stm      : "variable" IDENTIFIER expression ( expression )* ";"
// expression_stm       : expression ";"
// 

struct parser
{
    size_t current;
    array<token> *tokens;
    memory_arena *arena;
};

static inline bool
parser_validate_token(parser *state, token_type type)
{
    
    token_type current_type = (*state->tokens)[state->current].type;
    if (current_type == token_type::END_OF_FILE) 
    {
        return false;
    }
    return (current_type == type);

}

static inline bool
parser_match_token(parser *state, token_type type)
{

    if (parser_validate_token(state, type))
    {
        state->current++;
        return true;
    }

    return false;

}

static inline token*
parser_consume_token(parser *state, token_type type)
{

    if (parser_validate_token(state, type))
    {
        token* current_token = &(*state->tokens)[state->current];
        state->current++;
        return current_token;
    }

    return NULL;

}

static inline token*
parser_get_current_token(parser *state)
{
    token* current_token = &(*state->tokens)[state->current];
    return current_token;
}

static inline token*
parser_get_previous_token(parser *state)
{
    token* previous_token = &(*state->tokens)[state->current - 1];
    return previous_token;
}

static inline expression*
parser_allocate_binary_node(expression *left, expression *right, token *literal, memory_arena *arena)
{

    expression *expr = memory_arena_push_type(arena, expression);
    expr->binary_expression.left = left;
    expr->binary_expression.right = right;
    expr->binary_expression.literal = literal;
    expr->node_type = ast_node_type::BINARY_EXPRESSION;
    return expr;

}

static inline expression*
parser_allocate_unary_node(expression *primary, token *literal, memory_arena *arena)
{
    
    expression *expr = memory_arena_push_type(arena, expression);
    expr->unary_expression.primary = primary;
    expr->unary_expression.literal = literal;
    expr->node_type = ast_node_type::UNARY_EXPRESSION;
    return expr;

}

static inline expression*
parser_allocate_grouping_node(expression *primary, memory_arena *arena)
{

    expression *expr = memory_arena_push_type(arena, expression);
    expr->unary_expression.primary = primary;
    expr->unary_expression.literal = NULL;
    expr->node_type = ast_node_type::GROUPING_EXPRESSION;
    return expr;

}

static inline expression*
parser_allocate_literal_node(token *literal, memory_arena *arena)
{

    expression *expr = memory_arena_push_type(arena, expression);
    expr->unary_expression.primary = NULL;
    expr->unary_expression.literal = literal;
    expr->node_type = ast_node_type::LITERAL_EXPRESSION;
    return expr;

}

static expression*
parser_recursively_descend_expression(parser *state, expression_type level)
{

    switch (level)
    {

        case expression_type::EXPRESSION:
        {

            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::EQUALITY);
            return expr;

        } break;

        case expression_type::EQUALITY:
        {

            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::COMPARISON);

            while (parser_match_token(state, token_type::EQUALS) ||
                   parser_match_token(state, token_type::NOT_EQUALS))
            {

                token *operation = &(*state->tokens)[state->current - 1];

                expression *right = parser_recursively_descend_expression(state, expression_type::COMPARISON);
                propagate_on_error(right);

                expression *branch = parser_allocate_binary_node(expr, right, operation, state->arena);
                expr = branch;

            }

            return expr;

        } break;

        case expression_type::COMPARISON:
        {

            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::TERM);

            while (parser_match_token(state, token_type::LESS_THAN) ||
                   parser_match_token(state, token_type::LESS_THAN_EQUALS) ||
                   parser_match_token(state, token_type::GREATER_THAN) ||
                   parser_match_token(state, token_type::GREATER_THAN_EQUALS))
            {

                token *operation = &(*state->tokens)[state->current - 1];

                expression *right = parser_recursively_descend_expression(state, expression_type::TERM);
                propagate_on_error(right);

                expression *branch = parser_allocate_binary_node(expr, right, operation, state->arena);
                expr = branch;

            }

            return expr;

        } break;

        case expression_type::TERM:
        {
            expression *expr = parser_recursively_descend_expression(state, expression_type::FACTOR);

            while (parser_match_token(state, token_type::PLUS) ||
                   parser_match_token(state, token_type::MINUS))
            {

                token *operation = &(*state->tokens)[state->current - 1];

                expression *right = parser_recursively_descend_expression(state, expression_type::FACTOR);
                propagate_on_error(right);

                expression *branch = parser_allocate_binary_node(expr, right, operation, state->arena);
                expr = branch;

            }

            return expr;

        } break;

        case expression_type::FACTOR:
        {
            expression *expr = parser_recursively_descend_expression(state, expression_type::UNARY);

            while (parser_match_token(state, token_type::MULTIPLY) ||
                   parser_match_token(state, token_type::DIVISION))
            {
                
                token *operation = &(*state->tokens)[state->current - 1];

                expression *right = parser_recursively_descend_expression(state, expression_type::UNARY);
                propagate_on_error(right);

                expression *branch = parser_allocate_binary_node(expr, right, operation, state->arena);
                expr = branch;

            }

            return expr;

        } break;

        case expression_type::UNARY:
        {

            if (parser_match_token(state, token_type::MINUS))
            {

                token *operation = &(*state->tokens)[state->current - 1];

                expression *expr = parser_recursively_descend_expression(state, expression_type::UNARY);
                propagate_on_error(expr);

                expression *branch = parser_allocate_unary_node(expr, operation, state->arena);
                return branch;

            }

            expression *expr = parser_recursively_descend_expression(state, expression_type::PRIMARY);
            return expr;

        } break;

        case expression_type::PRIMARY:
        {

            // Primary elements.
            if (parser_match_token(state, token_type::NUMBER) ||
                parser_match_token(state, token_type::STRING) ||
                parser_match_token(state, token_type::IDENTIFIER))
            {

                token *literal = &(*state->tokens)[state->current - 1];
                expression *primary = parser_allocate_literal_node(literal, state->arena);
                return primary;

            }

            // Groupings.
            if (parser_match_token(state, token_type::LEFT_PARENTHESIS))
            {

                // We need to manually adjust the node type to grouping, as it carries
                // meaning for the transpilation process.
                expression *group = parser_recursively_descend_expression(state, expression_type::EXPRESSION);
                propagate_on_error(group);

                if (!parser_match_token(state, token_type::RIGHT_PARENTHESIS))
                {
                    parser_display_error(parser_get_previous_token(state), 
                            "expected ')' in expression.");
                    return NULL;
                }

                expression *expr = parser_allocate_grouping_node(group, state->arena);
                return expr;

            }

            parser_display_error(parser_get_current_token(state), 
                    "Unrecognized symbol in expression.");
            return NULL;

        } break;

    };

    assert(!"Unreachable condition, recursive routine did not catch all cases!");
    return NULL;

}

static statement*
parser_recursively_descend_statement(parser *state, statement_type level)
{

    switch (level)
    {

        case statement_type::STATEMENT:
        {
            
            // Declaration statements.
            if (parser_match_token(state, token_type::VARIABLE))
            {
                statement *stm = parser_recursively_descend_statement(state,
                        statement_type::DECLARATION_STATEMENT);
                return stm;
            }

            // Assume that the default is expression statements, so we fallthrough
            // on this case and let errors propogate back up.
            statement *stm = parser_recursively_descend_statement(state, 
                    statement_type::EXPRESSION_STATEMENT);
            return stm;

        } break;

        case statement_type::DECLARATION_STATEMENT:
        {
            
            token* identifier = parser_consume_token(state, token_type::IDENTIFIER);       
            if (identifier == NULL)
            {
                printf("Expected identifier after declaration statement.\n");
                propagate_on_error(identifier);
            }

            expression *size = parser_recursively_descend_expression(state,
                    expression_type::EXPRESSION);
            propagate_on_error(size);

            statement *stm = memory_arena_push_type(state->arena, statement);
            stm->node_type = ast_node_type::DECLARATION_STATEMENT;
            stm->declaration_statement.identifier = identifier;
            stm->declaration_statement.size = size;
            stm->declaration_statement.dimension_count = 0;

            // NOTE(Chris): We are going to match until we hit a semicolon
            //              or the max variable dimension count. I am not exactly
            //              sure what the COSY spec expects this to be, documentation
            //              doesn't put a capacity on it, but I am not exactly sure
            //              dimensions beyond 3 is a reasonable amount (n^c sizing).
            while (!parser_match_token(state, token_type::SEMICOLON))
            {
                expression *dimension = parser_recursively_descend_expression(state,
                        expression_type::EXPRESSION);
                propagate_on_error(dimension);
                
                size_t count = stm->declaration_statement.dimension_count;
                stm->declaration_statement.dimensions[count] = dimension;
                stm->declaration_statement.dimension_count++;

                if (stm->declaration_statement.dimension_count >
                        PARSER_VARIABLE_MAX_DIMENSIONS)
                {
                    printf("Max dimensions reached in variable declaration.\n");
                    propagate_on_error(NULL);
                }

            }

            return stm;

        } break;

        case statement_type::EXPRESSION_STATEMENT:
        {
            
            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::EXPRESSION);
            propagate_on_error(expr);

            if (!parser_match_token(state, token_type::SEMICOLON))
            {
                printf("Expected semicolon at end of statement.\n");
                return NULL;
            };

            statement *stm = memory_arena_push_type(state->arena, statement);
            stm->expression_statement.expr = expr;
            stm->node_type = ast_node_type::EXPRESSION_STATEMENT;
            return stm;

        } break;

    };

    printf("Unrecognized symbol in statement.\n");
    return NULL;

}

static inline void
parser_synchronize_state(parser *state)
{

    // Move forward until we synchronize to a valid position.
    token_type current_type = (*state->tokens)[state->current].type;
    while (current_type != token_type::SEMICOLON &&
           current_type != token_type::END_OF_FILE)
    {
        state->current += 1;
        current_type = (*state->tokens)[state->current].type;
    }

    // If we hit a semicolon, move passed it.
    if (current_type == token_type::SEMICOLON)
        state->current++;

    return;
}

bool
parser_generate_abstract_syntax_tree(array<token> *tokens, array<statement*> *statements, memory_arena *arena)
{

    assert(tokens != NULL);
    assert(statements != NULL);
    assert(arena != NULL);

    parser parser_state = {};
    parser_state.tokens = tokens;
    parser_state.current = 0;
    parser_state.arena = arena;

    bool encountered_error = false;
    while ((*tokens)[parser_state.current].type != token_type::END_OF_FILE)
    {

        statement *current_statement = parser_recursively_descend_statement(&parser_state,
                statement_type::STATEMENT);

        if (current_statement == NULL)
        {
            encountered_error = true;
            parser_synchronize_state(&parser_state); 
            continue;
        }

        statements->push(current_statement);

    };

    return (!encountered_error);

};

// --- Parser Print Traversal --------------------------------------------------
//
// This is a standard print traversal. Since the depth of this tree is somewhat
// complicated, the traversal is complicated. Handling this requires some care,
// so this routine is designed to demonstrate how to write a traversal for an AST.
// 

static inline void
parser_ast_traversal_print_expression(expression *expr)
{

    switch (expr->node_type)
    {

        case ast_node_type::BINARY_EXPRESSION:
        {
            
            parser_ast_traversal_print_expression(expr->binary_expression.left);

            string operation_string = parser_token_to_string(expr->binary_expression.literal);
            printf(" %s ", operation_string.str());

            parser_ast_traversal_print_expression(expr->binary_expression.right);

        } break;

        case ast_node_type::UNARY_EXPRESSION:
        {

            string operation_string = parser_token_to_string(expr->unary_expression.literal);
            printf("%s", operation_string.str());

            parser_ast_traversal_print_expression(expr->unary_expression.primary);

        } break;

        case ast_node_type::GROUPING_EXPRESSION:
        {

            printf("( ");

            parser_ast_traversal_print_expression(expr->unary_expression.primary);

            printf(" )");

        } break;

        case ast_node_type::LITERAL_EXPRESSION:
        {

            string literal_string = parser_token_to_string(expr->unary_expression.literal);
            printf("%s", literal_string.str());

        } break;

    }

    return;

}

static inline void
parser_ast_traversal_print_statement(statement *stm)
{

    switch (stm->node_type)
    {

        case ast_node_type::EXPRESSION_STATEMENT:
        {

            parser_ast_traversal_print_expression(stm->expression_statement.expr);
            printf(";\n");

            return;

        } break;

        case ast_node_type::DECLARATION_STATEMENT:
        {

            string variable_name = parser_token_to_string(stm->declaration_statement.identifier);
            printf("let %s ", variable_name.str());

            printf("( ");
            parser_ast_traversal_print_expression(stm->declaration_statement.size);
            printf(" )");

            for (size_t idx = 0; idx < stm->declaration_statement.dimension_count; ++idx)
            {
                printf("[");
                parser_ast_traversal_print_expression(stm->declaration_statement.dimensions[idx]);
                printf("]");
            };

            printf(";\n");

        } break;

        default:
        {
            assert(!"Uncaught AST traversal method.\n");
        } break;

    }

    return;

}

void
parser_ast_traversal_print(array<statement*> *statements)
{

    for (size_t idx = 0; idx < statements->size(); ++idx)
    {

        statement* current_statement = (*statements)[idx];
        parser_ast_traversal_print_statement(current_statement);

    }

}

// --- AST Traversals ----------------------------------------------------------
//
// The following definitions are AST traversal routines. Traversing isn't complicated,
// but does require the use of recursion and switch statements.
//

void
parser_ast_print_traversal(ast_node *ast)
{

    assert(ast != NULL); // Ensures we don't have an invalid ast.

    switch (ast->node_type)
    {
    
        case node_type::BINARY_NODE:
        {
            parser_ast_print_traversal(ast->left_expression);

            string token_string = parser_token_to_string(ast->operation);
            printf(" %s ", token_string.str());

            parser_ast_print_traversal(ast->right_expression);
            break;
        };

        case node_type::GROUPING_NODE:
        {
            
            printf("( ");
            parser_ast_print_traversal(ast->expression);
            printf(" )");

            break;
        };

        case node_type::UNARY_NODE:
        {
            
            string token_string = parser_token_to_string(ast->operation);
            printf("%s ", token_string.str());
            parser_ast_print_traversal(ast->expression);

            break;
        };

        case node_type::LITERAL_NODE:
        {
            string token_string = parser_token_to_string(ast->literal);
            printf("%s", token_string.str());

            break;
        };

    };

}

void
parser_ast_print_order_traversal(ast_node *ast)
{

    assert(ast != NULL); // Ensures we don't have an invalid ast.

    switch (ast->node_type)
    {
    
        case node_type::BINARY_NODE:
        {
            printf("( ");
            parser_ast_print_order_traversal(ast->left_expression);

            string token_string = parser_token_to_string(ast->operation);
            printf(" %s ", token_string.str());

            parser_ast_print_order_traversal(ast->right_expression);
            printf(" )");

            break;
        };

        case node_type::GROUPING_NODE:
        {
            
            printf("( ");
            parser_ast_print_order_traversal(ast->expression);
            printf(" )");

            break;
        };

        case node_type::UNARY_NODE:
        {
            
            string token_string = parser_token_to_string(ast->operation);
            printf("%s ", token_string.str());
            parser_ast_print_order_traversal(ast->expression);

            break;
        };

        case node_type::LITERAL_NODE:
        {
            string token_string = parser_token_to_string(ast->literal);
            printf("%s", token_string.str());

            break;
        };

    };

}

void
parser_ast_free_traversal(ast_node *ast)
{

    assert(ast != NULL); // Ensures we don't have an invalid ast.

    // Essentially a post-order traversal.
    switch (ast->node_type)
    {
    
        case node_type::BINARY_NODE:
        {
            parser_ast_free_traversal(ast->left_expression);
            parser_ast_free_traversal(ast->right_expression);
            memory_free(ast);
            break;
        };

        case node_type::GROUPING_NODE:
        {          
            parser_ast_free_traversal(ast->expression);
            memory_free(ast);
            break;
        };

        case node_type::UNARY_NODE:
        {         
            parser_ast_free_traversal(ast->expression);
            memory_free(ast);
            break;
        };

        case node_type::LITERAL_NODE:
        {
            memory_free(ast);
            break;
        };

    };


}
