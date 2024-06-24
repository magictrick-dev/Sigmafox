#include <cstdio>
#include <compiler/parser.h>
#include <string>
#include <stack>
#include <unordered_map>

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
        case token_type::SCOPE: return "scope";
        case token_type::ENDSCOPE: return "endscope";
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

        list_initialized = true;
    }

    // NOTE(Chris): String compares are somewhat expensive, so the trick is to
    //              reduce the amount of compares by alphabetizing into buckets
    //              and searching that way.
    for (size_t idx = 0; idx < 23; ++idx)
    {
        if (keyword_list[idx] == current)
            return type_list[idx];
    }

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
// expression   : assignment
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
// statement            : declaration_stm | expression_stm | block_stm
// block_stm            : "scope" statement* "endscope" ;
// declaration_stm      : "variable" IDENTIFIER expression ( expression )* ;
// expression_stm       : expression ;
// 

// --- Environments ------------------------------------------------------------
//
// Environments contain the symbol table? We're using C++ maps for this, but we
// will probably want a custom hashing routine to handle this. There's a good chance
// STL hashing routine isn't the fastest or least-resist to collisions, but for now
// it is the most performant option.
//

enum class symbol_type : uint32_t
{
    UNINITIALIZED,
    REAL,
    STRING,
};

struct symbol
{
    symbol_type type;
    token *token;
};

struct scope
{
    uint32_t depth;

    std::unordered_map<std::string, symbol> table;
    scope *parent_scope;
};

struct environment
{
    scope   global_scope;
    scope  *active_scope;
};

static inline void
parser_environment_push_scope(environment *env)
{

    scope *new_scope = memory_alloc_type(scope);
    new (new_scope) scope;
    new_scope->parent_scope = env->active_scope;
    new_scope->depth = new_scope->parent_scope->depth + 1;
    env->active_scope = new_scope;
}

static inline void
parser_environment_pop_scope(environment *env)
{
    scope *parent = env->active_scope->parent_scope;
    scope *current = env->active_scope;
    env->active_scope = parent;
    assert(parent != NULL); // We should never have a null scope, global must persist.
    current->~scope(); // Required for placement new due to std::unordered_map...
    free(current);
}

static inline symbol*
parser_environment_get_symbol(environment *env, token *identifier)
{

    std::string key;
    for (size_t idx = 0; idx < identifier->length; ++idx)
    {
        char c = *(identifier->source + identifier->offset_from_start + idx);
        key += c;
    }

    // Start at top-level scope and search for symbol.
    scope *current_scope = env->active_scope;
    while (current_scope != NULL)
    {
        auto search = current_scope->table.find(key); // ew, auto
        if (search != current_scope->table.end())
            return &search->second;

        current_scope = current_scope->parent_scope;
    }

    // If we didn't find it, we automatically return NULL.
    return NULL;

}

static inline symbol*
parser_environment_insert_symbol(environment *env, token *identifier)
{

    std::string key;
    for (size_t idx = 0; idx < identifier->length; ++idx)
    {
        char c = *(identifier->source + identifier->offset_from_start + idx);
        key += c;
    }

    scope *current_scope = env->active_scope;
    current_scope->table[key] = { symbol_type::UNINITIALIZED, identifier };
    return &current_scope->table.at(key);

}

//
// We will need to abstract some of this code out or refactor the connectedness
// between the scanner routine and the parser. As it turns out, we only look at
// the current token and the previous token and we store the ones that need to
// persist as pointer into the token list; this means that the token list is
// ultimately redundant.
//
// -----------------------------------------------------------------------------

struct parser
{
    size_t current;
    array<token> *tokens;
    memory_arena *arena;
    environment global_environment;
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
parser_allocate_assignment_node(token *identifier, expression *assignment, memory_arena *arena)
{
    expression *expr = memory_arena_push_type(arena, expression);
    expr->node_type = ast_node_type::ASSIGNMENT_EXPRESSION;
    expr->assignment_expression.identifier = identifier;
    expr->assignment_expression.assignment = assignment;
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
                    expression_type::ASSIGNMENT);
            return expr;

        } break;

        case expression_type::ASSIGNMENT:
        {

            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::EQUALITY);

            if (parser_match_token(state, token_type::ASSIGNMENT))
            {

                expression *assign = parser_recursively_descend_expression(state,
                        expression_type::ASSIGNMENT);
                propagate_on_error(assign);

                if (expr->node_type == ast_node_type::LITERAL_EXPRESSION)
                {

                    token *identifier = expr->unary_expression.literal;

                    expression *ass_expr = parser_allocate_assignment_node(identifier,
                            assign, state->arena);
                    return ass_expr;

                }

                parser_display_error(parser_get_previous_token(state), 
                        "Invalid assignment expression.");
                propagate_on_error(NULL);

            }

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

                // If the symbol is an identifier, verify its in the symbol table.
                if (literal->type == token_type::IDENTIFIER)
                {
                    symbol *sym = parser_environment_get_symbol(&state->global_environment, literal);
                    if (sym == NULL)
                    {
                        parser_display_error(literal, "Undefined identifier in expression.");
                        propagate_on_error(sym);
                    }
                }

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

            // Block statements.
            if (parser_match_token(state, token_type::SCOPE))
            {

                statement *stm = parser_recursively_descend_statement(state,
                        statement_type::BLOCK_STATEMENT);
                return stm;

            }

            // Expression statements.
            statement *stm = parser_recursively_descend_statement(state, 
                    statement_type::EXPRESSION_STATEMENT);
            return stm;

        } break;

        case statement_type::DECLARATION_STATEMENT:
        {
            
            token* identifier = parser_consume_token(state, token_type::IDENTIFIER);       
            if (identifier == NULL)
            {
                parser_display_error(parser_get_current_token(state), 
                        "Expected identifier after declaration statement.\n");
                propagate_on_error(identifier);
            }

            // Ensure the identifier isn't already declared in the current scope.
            symbol *sym = parser_environment_get_symbol(&state->global_environment, identifier);
            if (sym != NULL)
            {
                parser_display_error(parser_get_current_token(state), 
                        "Variable redeclared within current scope.");
                propagate_on_error(NULL);
            }
            sym = parser_environment_insert_symbol(&state->global_environment, identifier);

            expression *size = parser_recursively_descend_expression(state,
                    expression_type::EXPRESSION);
            propagate_on_error(size);
            if (size->node_type == ast_node_type::ASSIGNMENT_EXPRESSION)
            {
                parser_display_error(parser_get_previous_token(state),
                        "Variable size assignment expression is not allowed.");
                propagate_on_error(NULL);
            }

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
                if (dimension->node_type == ast_node_type::ASSIGNMENT_EXPRESSION)
                {
                    parser_display_error(parser_get_previous_token(state),
                            "Assignment expression is not allowed in array format.");
                    propagate_on_error(NULL);
                }
                
                size_t count = stm->declaration_statement.dimension_count;
                stm->declaration_statement.dimensions[count] = dimension;
                stm->declaration_statement.dimension_count++;

                if (stm->declaration_statement.dimension_count >
                        PARSER_VARIABLE_MAX_DIMENSIONS)
                {
                    parser_display_error(parser_get_current_token(state),
                            "Max dimensions reached in variable declaration.");
                    propagate_on_error(NULL);
                }

            }

            // Assuming we reached this point, the previous token *must* be a
            // semicolon, otherwise something got horribly messed up.
            token *previous = parser_get_previous_token(state);
            if (previous->type != token_type::SEMICOLON)
            {
                parser_display_error(previous, "Expected semicolon at end-of-line.");
                propagate_on_error(NULL);
            }

            return stm;

        } break;

        case statement_type::BLOCK_STATEMENT:
        {

            assert(!"No implementation");
            return NULL;

        } break;

        case statement_type::EXPRESSION_STATEMENT:
        {
            
            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::EXPRESSION);
            propagate_on_error(expr);

            if (!parser_match_token(state, token_type::SEMICOLON))
            {
                parser_display_error(parser_get_current_token(state),
                        "Expected semicolon at end of statement.");
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
    parser_state.global_environment.active_scope = 
        &parser_state.global_environment.global_scope;

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

        case ast_node_type::ASSIGNMENT_EXPRESSION:
        {
            string identifier = parser_token_to_string(expr->assignment_expression.identifier);
            printf("%s = ", identifier.str());

            parser_ast_traversal_print_expression(expr->assignment_expression.assignment);
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
parser_ast_traversal_print_statement(statement *stm, size_t depth)
{

    switch (stm->node_type)
    {

        case ast_node_type::EXPRESSION_STATEMENT:
        {

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            parser_ast_traversal_print_expression(stm->expression_statement.expr);
            printf(";\n");

            return;

        } break;

        case ast_node_type::DECLARATION_STATEMENT:
        {

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("sigmafox::dynamic<");
            parser_ast_traversal_print_expression(stm->declaration_statement.size);

            for (size_t idx = 0; idx < stm->declaration_statement.dimension_count; ++idx)
            {
                printf(", ");
                parser_ast_traversal_print_expression(stm->declaration_statement.dimensions[idx]);
            };

            printf(">");

            string variable_name = parser_token_to_string(stm->declaration_statement.identifier);
            printf(" %s()", variable_name.str());

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

    printf("#include <iostream>\n");
    printf("#include <sigmafox/core.h>\n\n");
    printf("int\nmain(int argc, char ** argv)\n{\n");
    for (size_t idx = 0; idx < statements->size(); ++idx)
    {

        statement* current_statement = (*statements)[idx];
        parser_ast_traversal_print_statement(current_statement, 4);

    }
    printf("}\n");

}

