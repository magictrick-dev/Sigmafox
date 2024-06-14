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
// expression   : equality
// equality     : comparison ( ( "=" | "#" ) comparison )*
// comparison   : term ( ( "<" | ">" | "<=" | ">=" ) term )*
// term         : factor ( ( "+" | "-" ) factor )*
// factor       : unary ( ( "*" | "/" ) unary )*
// unary        : ( "-" ) unary | primary
// primary      : NUMBER | STRING | "true" | "false" | "(" expression ")"

struct parser
{
    size_t current;
    array<token> *tokens;
    array<void*> *alloc_list;
};

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


static ast_node*
parser_recursive_descent_comparison(parser *state)
{

    return NULL;
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

        state->alloc_list->push(right);
    }

    state->alloc_list->push(expression);
    return expression;

}

static ast_node*
parser_recursive_descent_expression(parser *state)
{
    
    ast_node *equality = parser_recursive_descent_equality(state);
    return equality;

}

bool
parser_create_ast(array<token> *tokens, ast_node **ast, array<void*> *alloc_list)
{

    assert(tokens != NULL);
    assert(ast != NULL);
    assert(alloc_list != NULL);

    // Setup the parser state.
    parser state = {};
    state.current = 0;
    state.tokens = tokens;
    state.alloc_list = alloc_list;

    // Using recursive descent, we generate an expression that follows the
    // language grammar as describe in the above routine description.
    ast_node *expression = parser_recursive_descent_expression(&state);
    if (expression != NULL)
        *ast = expression; // Set the AST to the expression.

    return true;
}
