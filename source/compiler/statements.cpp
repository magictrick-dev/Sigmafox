#include <stdio.h>
#include <compiler/statements.h>
#include <compiler/environment.h>

statement*
parser_recursively_descend_statement(parser_state *state, statement_type level)
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

            // If statements.
            if (parser_match_token(state, token_type::IF))
            {
                statement *stm = parser_recursively_descend_statement(state,
                        statement_type::IF_STATEMENT);
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

            // If statements.
            if (parser_match_token(state, token_type::IF))
            {
                statement *stm = parser_recursively_descend_statement(state,
                        statement_type::IF_STATEMENT);
                return stm;
            }

            // While statements.
            if (parser_match_token(state, token_type::WHILE))
            {
              
                statement *stm = parser_recursively_descend_statement(state,
                        statement_type::WHILE_STATEMENT);
                return stm;

            }

            // For statements.
            if (parser_match_token(state, token_type::LOOP))
            {
                statement *stm = parser_recursively_descend_statement(state,
                        statement_type::FOR_STATEMENT);
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

        case statement_type::IF_STATEMENT:
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

            while (!parser_match_token(state, token_type::ENDIF) &&
                   !parser_match_token(state, token_type::ELSEIF))
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
                llnode *stm_node = linked_list_push_node(&stm->if_statement.if_block, 
                        state->arena);
                stm_node->data = scope_stm;
                

            }

            if (parser_check_token(state, token_type::END_OF_FILE))
            {
                parser_display_error(parser_get_previous_token(state),
                        "Unexpected end-of-file, unmatched ENDIF declaration?");
                return NULL;
            }


            while (parser_check_token(state, token_type::ELSEIF))
            {
                
                statement *else_stm = parser_recursively_descend_statement(state,
                        statement_type::ELSEIF_STATEMENT);
                propagate_on_error(else_stm);
            
                llnode *stm_node = linked_list_push_node(&stm->if_statement.elseif_statements,
                        state->arena);
                stm_node->data = else_stm;

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

        case statement_type::FOR_STATEMENT:
        {

            statement *stm = memory_arena_push_type(state->arena, statement);
            stm->node_type = ast_node_type::FOR_STATEMENT;
            environment_push_table(&state->global_environment);

            // Initial identifier name.
            token *identifier = parser_consume_token(state, token_type::IDENTIFIER);
            propagate_on_error(identifier);
            symbol *loop_variable = environment_add_symbol(&state->global_environment, identifier);

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
