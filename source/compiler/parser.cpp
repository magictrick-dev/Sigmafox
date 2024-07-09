#include <cstdio>
#include <compiler/parser.h>
#include <compiler/environment.h>
#include <string>
#include <stack>
#include <unordered_map>

// --- Error Handling ----------------------------------------------------------
//
// Although in its infancy, error handling is an important part of the parser that
// needs to be done with some care to ensure consistency with each error being
// displayed to the user. For now, we just let things go since we're in the
// early stages.
//

#define propagate_on_error(exp) if ((exp) == NULL) return NULL

#define ERROR_CODE_TOKENIZER_UNDEFINED_SYMBOL       "0x0001000A"
#define ERROR_CODE_TOKENIZER_EOL_REACH              "0x0001000B"
#define ERROR_CODE_TOKENIZER_EOF_REACH              "0x0001000C"
#define ERROR_CODE_SYNTAX_NO_SEMICOLON              "0x001000A0"
#define ERROR_CODE_SYNTAX_UNEXPECTED_SYMBOL         "0x001000B0"
#define ERROR_CODE_SYNTAX_MISSING_SYMBOL            "0x001000C0"
#define ERROR_CODE_SYNTAX_INVALID_EXPRESSION        "0x01000A00"

static inline void
parser_display_error(token *location, const char *reason)
{
    
    uint32_t column = token_column_number(location);
    uint32_t line = token_line_number(location);
    printf("%s(%d,%d) ERROR: %s\n", location->location, line, column, reason);

}

static inline void
parser_display_warning(token *location, const char *reason)
{

    uint32_t column = token_column_number(location);
    uint32_t line = token_line_number(location);
    printf("%s(%d,%d) WARNING: %s\n", location->location, line, column, reason);

}

// --- Abstract Syntax Tree ----------------------------------------------------
//
// An abstract syntax tree takes the set of linear tokens and composes them into
// a tree that corresponds to the language grammar. The basis for the language is
// expressions. We use recursive descent to the generate the required grammar.
// 
// Important Grammar Notes
//
//      comment_stm:    Comments are among a few special cases in which we actually
//                      want to carray over into the AST for transpilation. We
//                      incorporate it into the syntax for this reason. Since comments
//                      are parsed as single tokens, their formatting carries over
//                      complete with new-lines and character formatting that was
//                      initially incorporated into the comment. This could potentially
//                      pause some unusual edge-cases that may need to be considered,
//                      but for now we let this go through as-is.
//
//      block_stm:      Block statements push a new scope onto the environment.
//                      The scope pops when the appropriate "endscope" is encountered.
//
//      while_stm:      The while loop follows traditional while-loop functionality
//                      by evaluating the truthy expressions.
//
//      loop_stm:       The syntax is strange, but expression 1 is the initial state,
//                      and expression 2 is the ending state, where the identifier is set
//                      to expression 1 and incremented by 1 (or the optional incremental expression)
//                      until it reaches expression 2. Loops can not prematurely exit,
//                      therefore, at the end of each iteration, it must reset the step
//                      to its initial value. Loops always iterate n-time, however big it
//                      was initially defined to be. The generating syntax needs to
//                      mimmick this behavior.
//
// Complete Language Grammar
//
//      program                 : statement* EOF
//      statement               : comment_stm | declaration_stm | expression_stm | block_stm |
//                                while_stm | loop_stm
//      comment_stm             : "comment" statement*
//      block_stm               : "scope" statement* "endscope" ;
//      while_stm               : "while" expression ; statement* "endwhile" ;
//      loop_stm                : "loop" identifier expression expression ( expression )? ; 
//                                statement* "endwhile" ;
//      declaration_stm         : "variable" IDENTIFIER expression ( expression )* ;
//      expression_stm          : expression ;
//
// Expression Grammar
//
//      expression              : assignment
//      assignment              : identifier ":=" assignment | equality
//      equality                : comparison ( ( "=" | "#" ) comparison )*
//      comparison              : term ( ( "<" | ">" | "<=" | ">=" ) term )*
//      term                    : factor ( ( "+" | "-" ) factor )*
//      factor                  : unary ( ( "*" | "/" ) unary )*
//      unary                   : ( "-" ) unary | primary
//      primary                 : NUMBER | STRING | identifier | "(" expression ")"
// 

struct parser
{
    bool errored;
    size_t current;
    array<token> *tokens;
    memory_arena *arena;
    environment global_environment;
};

static inline bool
parser_validate_token(parser *state, token_type type)
{
    
    uint32_t current_type = (*state->tokens)[state->current].type;
    if (current_type == token_type::END_OF_FILE) 
    {
        return false;
    }
    return (current_type == type);

}

static inline bool
parser_check_token(parser *state, token_type type)
{
    uint32_t current_type = (*state->tokens)[state->current].type;
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


static inline void
parser_synchronize_state(parser *state)
{

    // Move forward until we synchronize to a valid position.
    uint32_t current_type = (*state->tokens)[state->current].type;
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
            propagate_on_error(expr);

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
                    symbol *sym = environment_get_symbol(&state->global_environment, literal);
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
            
            // Comment statements.
            if (parser_check_token(state, token_type::COMMENT_BLOCK))
            {
                statement *stm = parser_recursively_descend_statement(state, 
                        statement_type::COMMENT_STATEMENT);
                return stm;
            }

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

                if (!parser_consume_token(state, token_type::SEMICOLON))
                {
                    parser_display_error(parser_get_previous_token(state),
                            "Expected semicolon at end of scope declaration.");
                    propagate_on_error(NULL);
                }
                
                statement *stm = parser_recursively_descend_statement(state,
                        statement_type::BLOCK_STATEMENT);
                return stm;

            }

            // Loop statements.
            if (parser_match_token(state, token_type::WHILE))
            {
              
                statement *stm = parser_recursively_descend_statement(state,
                        statement_type::WHILE_STATEMENT);
                return stm;

            }

            // Expression statements.
            statement *stm = parser_recursively_descend_statement(state, 
                    statement_type::EXPRESSION_STATEMENT);
            return stm;

        } break;

        case statement_type::COMMENT_STATEMENT:
        {

            statement *comment_statement = memory_arena_push_type(state->arena, statement);
            comment_statement->node_type = ast_node_type::COMMENT_STATEMENT;
            comment_statement->comment_statement.comment = parser_consume_token(state,
                    token_type::COMMENT_BLOCK);
            return comment_statement;

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
            symbol *sym = environment_get_symbol(&state->global_environment, identifier);
            if (sym != NULL)
            {
                if (sym->depth == state->global_environment.depth)
                {
                    parser_display_error(parser_get_current_token(state), 
                            "Variable redeclared within current scope.");
                    propagate_on_error(NULL);
                }
                else 
                {
                    parser_display_warning(parser_get_current_token(state),
                            "Variable declaration shadows outer scope declaration.");
                }
            }

            sym = environment_add_symbol(&state->global_environment, identifier);

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

            // Block statements basically just recurse until we reach
            // the appropriate ENDSCOPE flag; any errors require synchronization
            // here, rather than propagating errors further up the recursion chain.
            statement *stm = memory_arena_push_type(state->arena, statement);
            stm->node_type = ast_node_type::BLOCK_STATEMENT;

            // When we encountered a block statement, we need to push a new scope
            // onto the environment.
            environment_push_table(&state->global_environment);

            while (!parser_match_token(state, token_type::ENDSCOPE))
            {
                statement *scope_stm = parser_recursively_descend_statement(state,
                        statement_type::STATEMENT);

                if (scope_stm == NULL)
                {
                    state->errored = true;
                    parser_synchronize_state(state); 
                    continue;
                }

                // NOTE(Chris): We don't actually have to allocate anything for
                //              the node, we can simply just set the data pointer
                //              to our statement.
                llnode *stm_node = linked_list_push_node(&stm->block_statement.statements, 
                        state->arena);
                stm_node->data = scope_stm;
                
            }

            if (parser_check_token(state, token_type::END_OF_FILE))
            {
                parser_display_error(parser_get_previous_token(state),
                        "Unexpected end-of-file, unmatched SCOPE declaration?");
                return NULL;
            }

            if (!parser_match_token(state, token_type::SEMICOLON))
            {
                // NOTE(Chris): We didn't hit EOF, which means that match_token actually
                //              reached ENDSCOPE, not EOF. Despite the lack of semicolon,
                //              we probably still want to pop the scope so further errors
                //              don't occur due to strange scopey behaviors.
                environment_pop_table(&state->global_environment);
                parser_display_error(parser_get_current_token(state),
                        "Expected semicolon at end of statement.");
                return NULL;
            }

            environment_pop_table(&state->global_environment);

            return stm;

        } break;

        case statement_type::WHILE_STATEMENT:
        {

            statement *stm = memory_arena_push_type(state->arena, statement);
            stm->node_type = ast_node_type::WHILE_STATEMENT;
            environment_push_table(&state->global_environment);

            // Check expression.
            expression *eval = parser_recursively_descend_expression(state, 
                    expression_type::EXPRESSION);
            propagate_on_error(eval);
            stm->while_statement.check = eval;

            // Following is a semicolon.
            if (!parser_consume_token(state, token_type::SEMICOLON))
            {
                parser_display_error(parser_get_previous_token(state),
                        "Expected semicolon at end of scope declaration.");
                propagate_on_error(NULL);
            }

            while (!parser_match_token(state, token_type::ENDWHILE))
            {
                statement *scope_stm = parser_recursively_descend_statement(state,
                        statement_type::STATEMENT);

                if (scope_stm == NULL)
                {
                    state->errored = true;
                    parser_synchronize_state(state); 
                    continue;
                }

                // NOTE(Chris): We don't actually have to allocate anything for
                //              the node, we can simply just set the data pointer
                //              to our statement.
                llnode *stm_node = linked_list_push_node(&stm->while_statement.statements, 
                        state->arena);
                stm_node->data = scope_stm;
                
            }

            if (parser_check_token(state, token_type::END_OF_FILE))
            {
                parser_display_error(parser_get_previous_token(state),
                        "Unexpected end-of-file, unmatched SCOPE declaration?");
                return NULL;
            }

            if (!parser_match_token(state, token_type::SEMICOLON))
            {
                // NOTE(Chris): We didn't hit EOF, which means that match_token actually
                //              reached ENDWHILE, not EOF. Despite the lack of semicolon,
                //              we probably still want to pop the scope so further errors
                //              don't occur due to strange scopey behaviors.
                environment_pop_table(&state->global_environment);
                parser_display_error(parser_get_current_token(state),
                        "Expected semicolon at end of statement.");
                return NULL;
            }

            environment_pop_table(&state->global_environment);

            return stm;

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

    assert(!"Unreachable condition, all statements should be caught.");
    printf("Unrecognized symbol in statement.\n");
    return NULL;

}

bool
parser_generate_abstract_syntax_tree(array<token> *tokens, array<statement*> *statements, memory_arena *arena)
{

    assert(tokens != NULL);
    assert(statements != NULL);
    assert(arena != NULL);

    parser parser_state = {};
    parser_state.errored = false;
    parser_state.tokens = tokens;
    parser_state.current = 0;
    parser_state.arena = arena;
    environment_push_table(&parser_state.global_environment);

    while ((*tokens)[parser_state.current].type != token_type::END_OF_FILE)
    {

        statement *current_statement = parser_recursively_descend_statement(&parser_state,
                statement_type::STATEMENT);

        if (current_statement == NULL)
        {
            parser_state.errored = true;
            parser_synchronize_state(&parser_state); 
            continue;
        }

        statements->push(current_statement);

    };

    environment_pop_table(&parser_state.global_environment);
    return (!parser_state.errored);

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

    char token_string_buffer[512];

    switch (expr->node_type)
    {

        case ast_node_type::BINARY_EXPRESSION:
        {
            
            parser_ast_traversal_print_expression(expr->binary_expression.left);

            token_copy_string(expr->binary_expression.literal, token_string_buffer, 260, 0);
            printf(" %s ", token_string_buffer);

            parser_ast_traversal_print_expression(expr->binary_expression.right);

        } break;

        case ast_node_type::UNARY_EXPRESSION:
        {

            token_copy_string(expr->unary_expression.literal, token_string_buffer, 260, 0);
            printf("%s", token_string_buffer);

            parser_ast_traversal_print_expression(expr->unary_expression.primary);

        } break;

        case ast_node_type::ASSIGNMENT_EXPRESSION:
        {
            token_copy_string(expr->assignment_expression.identifier, token_string_buffer, 260, 0);
            printf("%s = ", token_string_buffer);

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

            token_copy_string(expr->unary_expression.literal, token_string_buffer, 260, 0);
            printf("%s", token_string_buffer);

        } break;

    }

    return;

}

static inline void
parser_ast_traversal_print_statement(statement *stm, size_t depth)
{

    char token_string_buffer[512];

    switch (stm->node_type)
    {

        case ast_node_type::EXPRESSION_STATEMENT:
        {

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            parser_ast_traversal_print_expression(stm->expression_statement.expr);
            printf(";\n");

            return;

        } break;

        case ast_node_type::COMMENT_STATEMENT:
        {
            printf("\n");
            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("/*");
            token_copy_string(stm->comment_statement.comment, token_string_buffer, 512, 0);
            printf("%s", token_string_buffer);
            printf("*/\n");
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

            token_copy_string(stm->declaration_statement.identifier, token_string_buffer, 512, 0);
            printf(" %s()", token_string_buffer);

            printf(";\n");

        } break;

        case ast_node_type::BLOCK_STATEMENT:
        {


            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("{\n");
            llnode *current_node = stm->block_statement.statements.head;
            while (current_node != NULL)
            {
                statement *current = (statement*)current_node->data;
                parser_ast_traversal_print_statement(current, depth + 4);
                current_node = current_node->next;
            }

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("}\n");

        } break;

        case ast_node_type::WHILE_STATEMENT:
        {

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("while (");
            parser_ast_traversal_print_expression(stm->while_statement.check);
            printf(")\n");

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("{\n");

            llnode *current_node = stm->while_statement.statements.head;
            while (current_node != NULL)
            {
                statement *current = (statement*)current_node->data;
                parser_ast_traversal_print_statement(current, depth + 4);
                current_node = current_node->next;
            }

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("}\n");

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

