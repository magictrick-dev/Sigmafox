#include <cstdio>
#include <compiler/parser.h>
#include <compiler/environment.h>

// --- Error Handling ----------------------------------------------------------
//
// Although in its infancy, error handling is an important part of the parser that
// needs to be done with some care to ensure consistency with each error being
// displayed to the user. For now, we just let things go since we're in the
// early stages.
//

void
parser_display_error(token *location, const char *reason)
{
    
    uint32_t column = token_column_number(location);
    uint32_t line = token_line_number(location);
    printf("%s(%d,%d) ERROR: %s\n", location->location, line, column, reason);

}

void
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
//      if_stm:         Both the if and elseif statements are tied together. Interestingly,
//      elseif_stm:     there is COSY equivalent for "OR" and "AND", which is kinda wack.
//                      These can be achieved through nesting, but that's not very pretty.
//                      Another thing is that there is no plain "ELSE" statement. Therefore,
//                      it relies on a chaining of "ELSEIF"s before "ENDIF" to get the affect
//                      desired. The simplest solution is the left the "IF" tree contain
//                      a list of ELSEIF branches rather than keeping a list of lists.
//                      The recursion depth can potentially explode, but that would require
//                      some gnarly elseif-blocks to get to that point.
//
//      procedure_stm:  The procedure statement is a bit unique. The actual grammar is
//                      easy to catch. However, invoking procedures is tricky. The head
//                      of the procedure statement is an identifier, which then needs
//                      to be check if:
//
//                          a. The procedure is in-scope.
//                          b. The parameters are appropriately defined.
//
//                      So effectively, we now have a new statement grammar to procedures,
//                      an invokeproc_stm or something along those lines to actually handle
//                      this. This behavior itself is fine since procedures don't exist
//                      in mathematical expressions.
//
// Complete Language Grammar
//
//      program                 : statement* EOF
//      statement               : comment_stm | declaration_stm | expression_stm | block_stm |
//                                while_stm | loop_stm | if_stm | procedure_stm | invokeproc_stm |
//      comment_stm             : "comment" statement*
//      procedure_stm           : "procedure" identifier (identifier*) ";"
//                                statement* "endprocedure" ;
//      block_stm               : "scope" statement* "endscope" ;
//      while_stm               : "while" expression ; statement* "endwhile" ;
//      loop_stm                : "loop" identifier expression expression ( expression )? ; 
//                                statement* "endwhile" ;
//      if_stm                  : "if" expression ; statement* (elseif_stm)* "endif" ;
//      elseif_stm              : "elseif" expression ; statement*
//      declaration_stm         : "variable" identifier expression ( expression )* ;
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
//      unary                   : ( "-" ) unary | call
//      call                    : primary ( "(" arguments? ")" )?
//      arguments               : expression ( "," expression )*
//      primary                 : NUMBER | STRING | identifier | "(" expression ")"
//
// Grammar TODO List:
//
//      - For-loop syntax.
//      - If-statement syntax.
//      - Procedure syntax.
//

bool
parser_validate_token(parser_state *state, token_type type)
{
    
    uint32_t current_type = (*state->tokens)[state->current].type;
    if (current_type == token_type::END_OF_FILE) 
    {
        return false;
    }
    return (current_type == type);

}

bool
parser_check_token(parser_state *state, token_type type)
{
    uint32_t current_type = (*state->tokens)[state->current].type;
    return (current_type == type);
}

bool
parser_match_token(parser_state *state, token_type type)
{

    if (parser_validate_token(state, type))
    {
        state->current++;
        return true;
    }

    return false;

}

token*
parser_consume_token(parser_state *state, token_type type)
{

    if (parser_validate_token(state, type))
    {
        token* current_token = &(*state->tokens)[state->current];
        state->current++;
        return current_token;
    }

    return NULL;

}

token*
parser_get_current_token(parser_state *state)
{
    token* current_token = &(*state->tokens)[state->current];
    return current_token;
}

token*
parser_get_previous_token(parser_state *state)
{
    token* previous_token = &(*state->tokens)[state->current - 1];
    return previous_token;
}


void
parser_synchronize_state(parser_state *state)
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

// --- Expressions -------------------------------------------------------------
//
// Expressions utilitize the full recursive descent pattern in order to ensure
// proper conformance to the language grammar. Reference the grammar outlined
// above for more information on how the traversal works.
// 

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
parser_allocator_call_node(token *identifier, memory_arena *arena)
{
    expression *expr = memory_arena_push_type(arena, expression);
    expr->node_type = ast_node_type::CALL_EXPRESSION;
    expr->assignment_expression.identifier = identifier;
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
            propagate_on_error(expr);

            while (parser_match_token(state, token_type::EQUALS) ||
                   parser_match_token(state, token_type::NOT_EQUALS))
            {

                token *operation = &(*state->tokens)[state->current - 1];

                expression *right = parser_recursively_descend_expression(state,
                        expression_type::COMPARISON);
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
            propagate_on_error(expr);

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
            propagate_on_error(expr);

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
            propagate_on_error(expr);

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

        case expression_type::CALL:
        {

            expression *expr = parser_recursively_descend_expression(state, expression_type::PRIMARY);
            propagate_on_error(expr);

#if 0
            if (parser_match_token(state, token_type::LEFT_PARENTHESIS))
            {
                
            }
#endif

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
                    symbol *sym = environment_get_symbol(&state->global_environment, literal, false);
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
                expression *group = parser_recursively_descend_expression(state,
                        expression_type::EXPRESSION);
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

// --- Statements --------------------------------------------------------------
//
// Statements are the core bread and butter of the parser. It consumes the linear
// list of tokens and conforms them to the language grammar.
//

statement*
parser_create_comment_statement(parser_state *state)
{

    statement *comment_statement = memory_arena_push_type(state->arena, statement);
    comment_statement->node_type = ast_node_type::COMMENT_STATEMENT;
    comment_statement->comment_statement.comment = parser_consume_token(state,
            token_type::COMMENT_BLOCK);
    return comment_statement;

}

statement*
parser_create_declaration_statement(parser_state *state)
{

    token* identifier = parser_consume_token(state, token_type::IDENTIFIER);       
    if (identifier == NULL)
    {
        parser_display_error(parser_get_current_token(state), 
                "Expected identifier after declaration statement.\n");
        propagate_on_error(identifier);
    }

    // Ensure the identifier isn't already declared in the current scope.
    symbol *sym = environment_get_symbol(&state->global_environment, identifier, false);
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

    sym = environment_add_symbol(&state->global_environment, identifier, false);

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

}

statement* 
parser_create_block_statement(parser_state *state)
{

    // Block statements basically just recurse until we reach
    // the appropriate ENDSCOPE flag; any errors require synchronization
    // here, rather than propagating errors further up the recursion chain.
    statement *stm = memory_arena_push_type(state->arena, statement);
    stm->node_type = ast_node_type::BLOCK_STATEMENT;

    // When we encountered a block statement, we need to push a new scope
    // onto the environment.
    environment_push_table(&state->global_environment);

    if (!parser_consume_token(state, token_type::SEMICOLON))
    {
        parser_display_error(parser_get_previous_token(state),
                "Expected semicolon at end of scope declaration.");
        propagate_on_error(NULL);
    }

    while (!parser_match_token(state, token_type::ENDSCOPE))
    {
        statement *scope_stm = parser_recursively_descend_statement(state);

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

}

statement*
parser_create_if_statement(parser_state *state)
{

    statement *stm = memory_arena_push_type(state->arena, statement);
    stm->node_type = ast_node_type::IF_STATEMENT;
    environment_push_table(&state->global_environment);

    // Check expression.
    expression *check = parser_recursively_descend_expression(state,
            expression_type::EXPRESSION);
    propagate_on_error(check);
    stm->if_statement.if_check = check;

    // Following is a semicolon.
    if (!parser_consume_token(state, token_type::SEMICOLON))
    {
        parser_display_error(parser_get_previous_token(state),
                "Expected semicolon at end of scope declaration.");
        propagate_on_error(NULL);
    }

    while (true)
    {

        uint32_t elseif_type = token_type::ELSEIF;
        uint32_t current_type = parser_get_current_token(state)->type;
        bool matched_endif  = parser_match_token(state, token_type::ENDIF);
        bool matched_elseif = parser_match_token(state, token_type::ELSEIF);
        if (matched_endif || matched_elseif)
        {
            break;
        }

        statement *scope_stm = parser_recursively_descend_statement(state);

        if (scope_stm == NULL)
        {
            state->errored = true;
            parser_synchronize_state(state); 
            continue;
        }

        // NOTE(Chris): We don't actually have to allocate anything for
        //              the node, we can simply just set the data pointer
        //              to our statement.
        llnode *stm_node = linked_list_push_node(&stm->if_statement.if_block, 
                state->arena);
        stm_node->data = scope_stm;
        

    }

    while ((parser_get_previous_token(state))->type == token_type::ELSEIF)
    {

        statement *elseif_stm = parser_create_elseif_statement(state);
        propagate_on_error(elseif_stm);

        llnode *else_node = linked_list_push_node(&stm->if_statement.elseif_statements,
                state->arena);
        else_node->data = elseif_stm;

    }

    if (parser_check_token(state, token_type::END_OF_FILE))
    {
        parser_display_error(parser_get_previous_token(state),
                "Unexpected end-of-file, unmatched ENDIF declaration?");
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

}

statement* 
parser_create_elseif_statement(parser_state *state)
{

    statement *stm = memory_arena_push_type(state->arena, statement);
    stm->node_type = ast_node_type::ELSEIF_STATEMENT;
    environment_push_table(&state->global_environment);

    // Check expression.
    expression *check = parser_recursively_descend_expression(state,
            expression_type::EXPRESSION);
    propagate_on_error(check);
    stm->elseif_statement.elseif_check = check;

    // Following is a semicolon.
    if (!parser_consume_token(state, token_type::SEMICOLON))
    {
        parser_display_error(parser_get_previous_token(state),
                "Expected semicolon at end of scope declaration.");
        propagate_on_error(NULL);
    }

    while (!parser_match_token(state, token_type::ENDIF) &&
           !parser_match_token(state, token_type::ELSEIF))
    {

        statement *scope_stm = parser_recursively_descend_statement(state);

        if (scope_stm == NULL)
        {
            state->errored = true;
            parser_synchronize_state(state); 
            continue;
        }

        // NOTE(Chris): We don't actually have to allocate anything for
        //              the node, we can simply just set the data pointer
        //              to our statement.
        llnode *stm_node = linked_list_push_node(&stm->elseif_statement.elseif_block, 
                state->arena);
        stm_node->data = scope_stm;
        

    }

    if (parser_check_token(state, token_type::END_OF_FILE))
    {
        parser_display_error(parser_get_previous_token(state),
                "Unexpected end-of-file, unmatched ENDIF declaration?");
        return NULL;
    }

    environment_pop_table(&state->global_environment);
    return stm;

}

statement* 
parser_create_while_statement(parser_state *state)
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
        statement *scope_stm = parser_recursively_descend_statement(state);

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

}

statement* 
parser_create_for_statement(parser_state *state)
{

    statement *stm = memory_arena_push_type(state->arena, statement);
    stm->node_type = ast_node_type::FOR_STATEMENT;
    environment_push_table(&state->global_environment);

    // Initial identifier name.
    token *identifier = parser_consume_token(state, token_type::IDENTIFIER);
    propagate_on_error(identifier);
    symbol *loop_variable = environment_add_symbol(&state->global_environment, identifier, false);

    // Get the start.
    expression *start = parser_recursively_descend_expression(state,
            expression_type::EXPRESSION);
    propagate_on_error(start);

    // Get the end.
    expression *end = parser_recursively_descend_expression(state,
            expression_type::EXPRESSION);
    propagate_on_error(end);

    // There is an optional increment we need to check for.
    expression *increment = NULL;
    if (!parser_check_token(state, token_type::SEMICOLON))
    {
        increment = parser_recursively_descend_expression(state,
                expression_type::EXPRESSION);
        propagate_on_error(increment);
    }

    // Following is a semicolon.
    if (!parser_consume_token(state, token_type::SEMICOLON))
    {
        parser_display_error(parser_get_previous_token(state),
                "Expected semicolon at end of scope declaration.");
        propagate_on_error(NULL);
    }

    stm->for_statement.identifier = identifier;
    stm->for_statement.start = start;
    stm->for_statement.end = end;
    stm->for_statement.increment = increment;

    while (!parser_match_token(state, token_type::ENDLOOP))
    {
        statement *scope_stm = parser_recursively_descend_statement(state);

        if (scope_stm == NULL)
        {
            state->errored = true;
            parser_synchronize_state(state); 
            continue;
        }

        // NOTE(Chris): We don't actually have to allocate anything for
        //              the node, we can simply just set the data pointer
        //              to our statement.
        llnode *stm_node = linked_list_push_node(&stm->for_statement.statements, 
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
        //              reached ENDLOOP, not EOF. Despite the lack of semicolon,
        //              we probably still want to pop the scope so further errors
        //              don't occur due to strange scopey behaviors.
        environment_pop_table(&state->global_environment);
        parser_display_error(parser_get_current_token(state),
                "Expected semicolon at end of statement.");
        return NULL;
    }

    environment_pop_table(&state->global_environment);

    return stm;

}

statement* 
parser_create_procedure_statement(parser_state *state)
{

    statement *stm = memory_arena_push_type(state->arena, statement);
    stm->node_type = ast_node_type::PROCEDURE_STATEMENT;
    environment_push_table(&state->global_environment);

    // Initial identifier name.
    token *identifier = parser_consume_token(state, token_type::IDENTIFIER);
    propagate_on_error(identifier);
    stm->procedure_statement.identifier = identifier;
    
    while (!parser_check_token(state, token_type::SEMICOLON))
    {

        token *parameter = parser_consume_token(state, token_type::IDENTIFIER);
        propagate_on_error(identifier);

        llnode *param_node = linked_list_push_node(&stm->procedure_statement.parameter_names,
                state->arena);
        param_node->data = parameter;

        // Add params to scope.
        symbol *param_sym = environment_get_symbol(&state->global_environment,
                parameter, false);
        if (param_sym != NULL)
        {
            parser_display_error(parameter, "Parameter redefinition in procedure def.");
            environment_pop_table(&state->global_environment);
            propagate_on_error(NULL);
        }

        param_sym = environment_add_symbol(&state->global_environment,
                parameter, false);

    }

    // Following is a semicolon.
    if (!parser_consume_token(state, token_type::SEMICOLON))
    {
        parser_display_error(parser_get_previous_token(state),
                "Expected semicolon at end of scope declaration.");
        propagate_on_error(NULL);
    }

    while (!parser_match_token(state, token_type::ENDPROCEDURE))
    {
        statement *scope_stm = parser_recursively_descend_statement(state);

        if (scope_stm == NULL)
        {
            state->errored = true;
            parser_synchronize_state(state); 
            continue;
        }

        // NOTE(Chris): We don't actually have to allocate anything for
        //              the node, we can simply just set the data pointer
        //              to our statement.
        llnode *stm_node = linked_list_push_node(&stm->procedure_statement.statements, 
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

    // Insert the procedure into the global scope.
    symbol *proc_symbol = environment_get_symbol(&state->global_environment,
            identifier, true);
    if (proc_symbol != NULL)
    {
        parser_display_error(identifier, "Procedure redefinition.");
        environment_pop_table(&state->global_environment);
        propagate_on_error(NULL);
    }

    proc_symbol = environment_add_symbol(&state->global_environment,
            identifier, true);

    return stm;

}

statement* 
parser_create_expression_statement(parser_state *state)
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

}

statement*
parser_recursively_descend_statement(parser_state *state)
{

    // Comment statements.
    if (parser_check_token(state, token_type::COMMENT_BLOCK))
        return parser_create_comment_statement(state);

    // Variable statements.
    if (parser_match_token(state, token_type::VARIABLE))
        return parser_create_declaration_statement(state);

    // Scope statements.
    if (parser_match_token(state, token_type::SCOPE))
        return parser_create_block_statement(state);

    // If statements.
    if (parser_match_token(state, token_type::IF))
        return parser_create_if_statement(state);

    // While statements.
    if (parser_match_token(state, token_type::WHILE))
        return parser_create_while_statement(state);

    // Loop statements.
    if (parser_match_token(state, token_type::LOOP))
        return parser_create_for_statement(state);

    if (parser_match_token(state, token_type::PROCEDURE))
        return parser_create_procedure_statement(state);

    // Expression statements.
    return parser_create_expression_statement(state);

}
