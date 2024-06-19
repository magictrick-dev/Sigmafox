#include <cstdio>
#include <compiler/parser.h>

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
        case token_type::PRINT: return "print";
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

static inline token
scanner_create_token(scanner *state, token_type type)
{

    token result = {};
    result.source = state->source;
    result.offset_from_start = state->start;
    result.offset_from_line = state->line_offset;
    result.length = state->step - state->start;
    result.line = state->line;
    result.type = type;
    return result;

}

bool 
parser_tokenize_source_file(const char *source_file, array<token> *tokens, array<token> *errors)
{

    scanner state;
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
// statement            : expression_stm | print_stm
// expression_stm       : expression ";"
// print_stm            : "print" expression ";"
// 

struct parser
{
    size_t current;
    array<token> *tokens;
    memory_arena *arena;
};

static ast_node* parser_recursive_descent_expression(parser *state);
static ast_node* parser_recursive_descent_equality(parser *state);
static ast_node* parser_recursive_descent_term(parser *state);
static ast_node* parser_recursive_descent_factor(parser *state);
static ast_node* parser_recursive_descent_unary(parser *state);
static ast_node* parser_recursive_descent_primary(parser *state);

inline ast_node*
parser_create_ast_binary_node(ast_node *left, ast_node *right, token *operation)
{

    assert(left != NULL);
    assert(right != NULL);
    assert(operation != NULL);

    ast_node *node = memory_alloc_type(ast_node);
    node->node_type         = node_type::BINARY_NODE;
    node->operation         = operation;
    node->left_expression   = left;
    node->right_expression  = right;
    return node;

}

inline ast_node*
parser_create_ast_grouping_node(ast_node *expression)
{

    assert(expression != NULL);

    ast_node *node = memory_alloc_type(ast_node);
    node->node_type     = node_type::GROUPING_NODE;
    node->operation     = NULL;
    node->expression    = expression;
    return node;

}

inline ast_node*
parser_create_ast_unary_node(ast_node *expression, token *operation)
{

    assert(expression != NULL);
    assert(operation != NULL);

    ast_node *node = memory_alloc_type(ast_node);
    node->node_type     = node_type::UNARY_NODE;
    node->operation     = operation;
    node->expression    = expression;
    return node;

}

inline ast_node*
parser_create_ast_literal_node(token *literal)
{
    
    assert(literal != NULL);

    ast_node *node = memory_alloc_type(ast_node);
    node->node_type     = node_type::LITERAL_NODE;
    node->literal       = literal;
    node->expression    = NULL;
    return node;

}

static inline token*
parser_advance(parser *state)
{
    if (state->current < state->tokens->size())
        state->current++;
    token *current = &(*state->tokens)[state->current];
    return current;
}

static inline token*
parser_previous(parser *state)
{
    token *previous = &(*state->tokens)[state->current - 1];
    return previous;
}

static inline token*
parser_peek(parser *state)
{
    token *next = &(*state->tokens)[state->current];
    return next;
}

static inline bool
parser_is_eof(parser *state)
{
    token *next = parser_peek(state);
    return next->type == token_type::END_OF_FILE;
}

static inline bool
parser_match(parser *state, token_type type)
{
    if (parser_is_eof(state))
        return false;
    else
    {
        token *next = parser_peek(state);
        if (next->type == type)
        {
            parser_advance(state);
            return true;
        }
        else
        {
            return false;
        }   
    }
}

static inline token*
parser_consume(parser *state, token_type check)
{

    if (parser_is_eof(state)) return NULL;

    token *next = parser_peek(state);
    if (next->type == check) return parser_advance(state);
    return NULL;

}

static ast_node*
parser_recursive_descent_primary(parser *state)
{

    if (parser_match(state, token_type::STRING) ||
        parser_match(state, token_type::NUMBER))
    {
        token *literal = parser_previous(state);
        ast_node *expression = parser_create_ast_literal_node(literal);
        return expression;
    }

    if (parser_match(state, token_type::LEFT_PARENTHESIS))
    {

        // Groupings will recurse up.
        ast_node *expression = parser_recursive_descent_expression(state);
        token *potential = parser_consume(state, token_type::RIGHT_PARENTHESIS);

        if (potential == NULL)
        {
            printf("Error, unmatched paranthesis in expression.\n");
            return NULL;
        }

        return expression;

    };

    // NOTE(Chris): When we reach this condition, we return NULL, indicating that
    //              there was an error. At no point should there be a NULL, so we
    //              assume we should consume to next statement.
    printf("Error, unexpected symbol in expression.\n");
    return NULL; 

}

static ast_node*
parser_recursive_descent_unary(parser *state)
{

    if (parser_match(state, token_type::MINUS))
    {
        token *operation = parser_previous(state);
        ast_node *right = parser_recursive_descent_unary(state);
        ast_node *expression = parser_create_ast_unary_node(right, operation);

        return expression;
    }

    ast_node *expression = parser_recursive_descent_primary(state);
    return expression;

}

static ast_node*
parser_recursive_descent_factor(parser *state)
{

    ast_node *expression = parser_recursive_descent_unary(state);

    while (parser_match(state, token_type::MULTIPLY) ||
           parser_match(state, token_type::DIVISION))
    {
        
        token *operation = parser_previous(state);
        ast_node *right = parser_recursive_descent_unary(state);
        expression = parser_create_ast_binary_node(expression, right, operation);

    }

    return expression;

}

static ast_node*
parser_recursive_descent_term(parser *state)
{

    ast_node *expression = parser_recursive_descent_factor(state);

    while (parser_match(state, token_type::PLUS) ||
           parser_match(state, token_type::MINUS))
    {
        
        token *operation = parser_previous(state);
        ast_node *right = parser_recursive_descent_factor(state);
        expression = parser_create_ast_binary_node(expression, right, operation);

    }

    return expression;

};

static ast_node*
parser_recursive_descent_comparison(parser *state)
{

    ast_node *expression = parser_recursive_descent_term(state);

    while (parser_match(state, token_type::LESS_THAN) ||
           parser_match(state, token_type::GREATER_THAN) ||
           parser_match(state, token_type::LESS_THAN_EQUALS) ||
           parser_match(state, token_type::GREATER_THAN_EQUALS))
    {
        
        token *operation = parser_previous(state);
        ast_node *right = parser_recursive_descent_term(state);
        expression = parser_create_ast_binary_node(expression, right, operation);

    }

    return expression;
}

static ast_node*
parser_recursive_descent_equality(parser *state)
{

    ast_node *expression = parser_recursive_descent_comparison(state);

    while (parser_match(state, token_type::EQUALS) || 
           parser_match(state, token_type::NOT_EQUALS))
    {

        token *operation = parser_previous(state);
        ast_node *right = parser_recursive_descent_comparison(state);
        expression = parser_create_ast_binary_node(expression, right, operation);

    }

    return expression;

}

static ast_node*
parser_recursive_descent_expression(parser *state)
{
    
    ast_node *equality = parser_recursive_descent_equality(state);
    return equality;

}

// --- AST Format Refactor -----------------------------------------------------
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
// statement            : expression_stm | print_stm
// expression_stm       : expression ";"
// print_stm            : "print" expression ";"

static inline bool
parser_validate_token(parser *state, token_type type)
{
    
    uint32_t current_type = (*state->tokens)[state->current].type;
    if (current_type == token_type::END_OF_FILE) return false;
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

static expression*
parser_recursively_descend_expression(parser *state, int level)
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
            return expr;

        } break;

        case expression_type::COMPARISON:
        {

            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::TERM);
            return expr;

        } break;

        case expression_type::TERM:
        {
            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::FACTOR);
            return expr;

        } break;

        case expression_type::FACTOR:
        {
            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::UNARY);
            return expr;

        } break;

        case expression_type::UNARY:
        {

            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::PRIMARY);
            return expr;

        } break;

        case expression_type::PRIMARY:
        {



        } break;

    };

    printf("Unrecognized symbol in expression.\n");
    return NULL;

}

static statement*
parser_recursively_descend_statement(parser *state, int level)
{

    switch (level)
    {

        case statement_type::STATEMENT:
        {
            
            // Default if we don't reach any other conditions.
            parser_recursively_descend_statement(state, 
                    statement_type::EXPRESSION_STATEMENT);

        } break;

        case statement_type::EXPRESSION_STATEMENT:
        {
            
            expression *expr = parser_recursively_descend_expression(state,
                    expression_type::EXPRESSION);

            if (parser_match_token(state, token_type::SEMICOLON))
            {
                printf("Expected semicolon at end of statement.\n");
                return NULL;
            };

            statement *stm = memory_arena_push_type(state->arena, statement);
            stm->expression_statement.expr = expr;
            stm->type = statement_type::EXPRESSION_STATEMENT;
            return stm;

        } break;

    };

    printf("Unrecognized symbol in statement.\n");
    return NULL;

}

bool
parser_generate_abstract_syntax_tree(array<token> *tokens, array<statement> *program, memory_arena *arena)
{

    assert(tokens != NULL);
    assert(program != NULL);
    assert(arena != NULL);

    parser parser_state = {};
    parser_state.tokens = tokens;
    parser_state.current = 0;
    parser_state.arena = arena;

    bool encountered_error = false;
    while ((*tokens)[parser_state.current].type != token_type::END_OF_FILE)
    {
        
    };

    return (!encountered_error);

};

/*
static statement_node*
parser_recursive_descent_expression_stm(parser *state)
{

    ast_node *general_expression = parser_recursive_descent_expression(state);
    

};

static statement_node*
parser_recursive_descent_statement(parser *state)
{
    
    if (parser_match(state, token_type::PRINT))
    {
        statement_node *print = parser_recursive_descent_print_stm(state);
        return print;
    }

    // Fall through condition, standard expression statement.
    statement_node *expression = parser_recursive_descent_expression_stm(state);
    return expression;

}

bool
parser_generate_ast(array<token> *tokens, array<statement_node> *statements, memory_arena *arena)
{

    assert(tokens != NULL);
    assert(statements != NULL);
    assert(arena != NULL);

    parser state = {};
    state.current = 0;
    state.tokens = tokens;
    state.arena = arena;

    bool parse_is_valid = true;
    while (!parser_is_eof(&state))
    {
        statement_node *current_statement = parser_recursive_descent_statement(&state);
        if (current_state != NULL) statements->push(current_statement);
        else parse_is_valid == false;
    }

    return parse_is_valid;

}
*/

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
