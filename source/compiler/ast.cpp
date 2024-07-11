#include <stdio.h>
#include <compiler/ast.h>
#include <compiler/parser.h>

// --- AST Parse Routine -------------------------------------------------------
//
//eParses and constructs the AST, returning back the list of statements generated
// from the tree.
//

bool
parse_tokens(array<token> *tokens, array<statement*> *statements, memory_arena *arena)
{

    assert(tokens != NULL);
    assert(statements != NULL);
    assert(arena != NULL);

    parser_state state = {};
    state.errored = false;
    state.tokens = tokens;
    state.current = 0;
    state.arena = arena;
    environment_push_table(&state.global_environment);

    while ((*tokens)[state.current].type != token_type::END_OF_FILE)
    {

        statement *current_statement = parser_recursively_descend_statement(&state,
                statement_type::STATEMENT);

        if (current_statement == NULL)
        {
            state.errored = true;
            parser_synchronize_state(&state); 
            continue;
        }

        statements->push(current_statement);

    };

    environment_pop_table(&state.global_environment);
    return (!state.errored);

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

        case ast_node_type::IF_STATEMENT:
        {

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("if (");
            parser_ast_traversal_print_expression(stm->if_statement.if_check);
            printf(")\n");

            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("{\n");

            llnode *current_node = stm->if_statement.if_block.head;
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

        case ast_node_type::FOR_STATEMENT:
        {

            token_copy_string(stm->for_statement.identifier, token_string_buffer, 512, 0);
            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("for (");
            printf("sigmafox::dynamic<4> %s = ", token_string_buffer);
            parser_ast_traversal_print_expression(stm->for_statement.start);
            printf("; %s < ", token_string_buffer);
            parser_ast_traversal_print_expression(stm->for_statement.end);
            printf("; ");

            if (stm->for_statement.increment == NULL)
            {
                printf("%s += 1)\n", token_string_buffer);
            }
            else
            {
                printf("%s += ", token_string_buffer);
                parser_ast_traversal_print_expression(stm->for_statement.increment);
                printf(")\n");
            }


            for (size_t idx = 0; idx < depth; ++idx) printf(" ");
            printf("{\n");

            for (size_t idx = 0; idx < depth + 4; ++idx) printf(" ");
            printf("sigmafox::dynamic<4> __loop_index_cache = %s;\n", token_string_buffer);

            llnode *current_node = stm->for_statement.statements.head;
            while (current_node != NULL)
            {
                statement *current = (statement*)current_node->data;
                parser_ast_traversal_print_statement(current, depth + 4);
                current_node = current_node->next;
            }

            for (size_t idx = 0; idx < depth + 4; ++idx) printf(" ");
            printf("%s = __loop_index_cache;\n", token_string_buffer);

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

