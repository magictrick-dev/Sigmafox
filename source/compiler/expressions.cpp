#include <compiler/expressions.h>
#include <compiler/environment.h>

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

expression*
parser_recursively_descend_expression(parser_state *state, expression_type level)
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
