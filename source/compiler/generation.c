#include <compiler/generation.h>

// --- AST Transpilation Routine -----------------------------------------------

void
transpile_scope_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    assert(root_node->type == SCOPE_STATEMENT_NODE);

    insert_text_at(file->body, arena, "\n");
    insert_tabbing_at(file->body, arena);
    insert_text_at(file->body, arena, "{\n\n");
    push_tabs_at(file->body);
        
        syntax_node *current_node = root_node->scope.body_statements;
        while(current_node != NULL)
        {
            
            transpile_node(current_node, file, arena);
            current_node = current_node->next_node;

        }

    pop_tabs_at(file->body);
    insert_text_at(file->body, arena, "\n");
    insert_tabbing_at(file->body, arena);
    insert_text_at(file->body, arena, "};\n\n");

}

void 
transpile_program_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    assert(root_node->type == PROGRAM_ROOT_NODE);

    insert_text_at(file->body, arena, "int\n");
    insert_text_at(file->body, arena, "main(int argc, char **argv)\n");
    insert_text_at(file->body, arena, "{\n\n");
    push_tabs_at(file->body);

        syntax_node *current_node = root_node->program.body_statements;
        while (current_node != NULL)
        {

            transpile_node(current_node, file, arena);
            current_node = current_node->next_node;

        }

    pop_tabs_at(file->body);
    insert_text_at(file->body, arena, "\n}\n");

}

void
transpile_variable_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    assert(root_node->type == VARIABLE_STATEMENT_NODE);

    insert_tabbing_at(file->body, arena);
    insert_text_at(file->body, arena, "int ");
    insert_text_at(file->body, arena, root_node->variable.name);

    if (root_node->variable.assignment != NULL)
    {
        insert_text_at(file->body, arena, " = ");
        transpile_node(root_node->variable.assignment, file, arena);
    }

    insert_text_at(file->body, arena, ";\n");

}

void 
transpile_binary_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    transpile_node(root_node->binary.left, file, arena);

    switch (root_node->binary.type)
    {

        case OPERATION_ADDITION:            insert_text_at(file->body, arena, " + "); break;
        case OPERATION_SUBTRACTION:         insert_text_at(file->body, arena, " - "); break;
        case OPERATION_MULTIPLICATION:      insert_text_at(file->body, arena, " * "); break;
        case OPERATION_DIVISION:            insert_text_at(file->body, arena, " / "); break;
        case OPERATION_EQUALS:              insert_text_at(file->body, arena, " == "); break;
        case OPERATION_NOT_EQUALS:          insert_text_at(file->body, arena, " != "); break;
        case OPERATION_LESS_THAN:           insert_text_at(file->body, arena, " < "); break;
        case OPERATION_LESS_THAN_EQUALS:    insert_text_at(file->body, arena, " <= "); break;
        case OPERATION_GREATER_THAN:        insert_text_at(file->body, arena, " > "); break;
        case OPERATION_GREATER_THAN_EQUALS: insert_text_at(file->body, arena, " >= "); break;
        case OPERATION_ASSIGNMENT:          insert_text_at(file->body, arena, " = "); break;
        
        default:
        {

            assert(!"Unimplemented operation for binary expression.");
            return;

        } break;
    }

    transpile_node(root_node->binary.right, file, arena);

}

void
transpile_unary_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    assert(root_node->type == UNARY_EXPRESSION_NODE);

    switch (root_node->unary.type)
    {

        case OPERATION_NEGATIVE_ASSOCIATE: insert_text_at(file->body, arena, "-"); break;

        default:
        {

            assert(!"Unimplemented operation for unary expression printing.");
            return;

        } break;

    };

    transpile_node(root_node->unary.right, file, arena);

}

void
transpile_grouping_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    assert(root_node->type == GROUPING_EXPRESSION_NODE);

    insert_text_at(file->body, arena, "( ");
        transpile_node(root_node->grouping.grouping, file, arena);
    insert_text_at(file->body, arena, " )");

}

void
transpile_assignment_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    assert(root_node->type == ASSIGNMENT_EXPRESSION_NODE);

    insert_text_at(file->body, arena, root_node->assignment.identifier);
    insert_text_at(file->body, arena, " = ");
    transpile_node(root_node->assignment.right, file, arena);
    insert_text_at(file->body, arena, ";\n");

}

void
transpile_primary_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    assert(root_node->type == PRIMARY_EXPRESSION_NODE);
    insert_text_at(file->body, arena, root_node->primary.literal.identifier);

}

void
transpile_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    switch(root_node->type)
    {
        
        case PROGRAM_ROOT_NODE:
        {
            transpile_program_node(root_node, file, arena);
            return;
        } break;

        case SCOPE_STATEMENT_NODE:
        {
            transpile_scope_node(root_node, file, arena);
            return; 
        } break;

        case VARIABLE_STATEMENT_NODE:
        {
            transpile_variable_node(root_node, file, arena);
            return;
        } break;

        case BINARY_EXPRESSION_NODE:
        {
            transpile_binary_node(root_node, file, arena);
            return;
        } break;

        case UNARY_EXPRESSION_NODE:
        {
            transpile_unary_node(root_node, file, arena);
            return;
        } break;

        case PRIMARY_EXPRESSION_NODE:
        {
            transpile_primary_node(root_node, file, arena);
            return;
        }

        case ASSIGNMENT_EXPRESSION_NODE:
        {
            transpile_assignment_node(root_node, file, arena);
            return;
        } return;

        case GROUPING_EXPRESSION_NODE:
        {
            transpile_grouping_node(root_node, file, arena);
            return;
        } break;

        default:
        {
            assert(!"Uncaught transpilation case for syntax node.");
            return;
        }

    };

}

void 
transpile_syntax_tree(syntax_node *root_node, memory_arena *arena, cc64 output_name)
{

    assert(root_node != NULL);

    source_file main_file = {0};
    main_file.file_name = output_name;
    main_file.header = memory_arena_push_type(arena, source_section);
    main_file.body = memory_arena_push_type(arena, source_section);
    main_file.footer = memory_arena_push_type(arena, source_section);

    main_file.header->start     = NULL;
    main_file.header->end       = NULL;
    main_file.header->tab_depth = 0;

    main_file.body->start       = NULL;
    main_file.body->end         = NULL;
    main_file.body->tab_depth   = 0;

    main_file.footer->start     = NULL;
    main_file.footer->end       = NULL;
    main_file.footer->tab_depth = 0;

    // We need to include some headers.
    insert_text_at(main_file.header, arena, "#include <iostream>\n");
    insert_text_at(main_file.header, arena, "#include <string>\n");
    insert_text_at(main_file.header, arena, "\n\n");

    // Traverse our program nodes.
    transpile_program_node(root_node, &main_file, arena);

    // Write the transpiled output to disk.
    void* write_handle = fileio_write_stream_open(output_name);
    if (write_handle == NULL)
    {
        printf("Invalid write handle.");
        return;
    }

    source_string *head_section = main_file.header->start;
    while (head_section != NULL)
    {
        fileio_write_stream_write(write_handle, head_section->buffer, head_section->size);
        head_section = head_section->next;
    }

    source_string *body_section = main_file.body->start;
    while (body_section != NULL)
    {
        fileio_write_stream_write(write_handle, body_section->buffer, body_section->size);
        body_section = body_section->next;
    }

    source_string *footer_section = main_file.footer->start;
    while (footer_section != NULL)
    {
        fileio_write_stream_write(write_handle, footer_section->buffer, footer_section->size);
        footer_section = footer_section->next;
    }

    fileio_write_stream_close(write_handle);

    return;

}

// --- Helpers -----------------------------------------------------------------

void 
push_tabs_at(source_section *section)
{
    section->tab_depth += TAB_SPACE_SIZE;
}

void 
pop_tabs_at(source_section *section)
{
    assert(section->tab_depth >= TAB_SPACE_SIZE);
    section->tab_depth -= TAB_SPACE_SIZE;
}

void
insert_tabbing_at(source_section *section, memory_arena *arena)
{

    u64 string_length = section->tab_depth;
    c64 string_buffer = memory_arena_push(arena, string_length + 1);
    for (u64 i = 0; i < string_length; ++i) string_buffer[i] = ' ';
    string_buffer[string_length] = '\0';

    source_string *insert = memory_arena_push_type(arena, source_string);
    insert->buffer = string_buffer;
    insert->size = string_length;
    insert->next = NULL;

    if (section->start == NULL)
    {
        section->start = insert;
        section->end = insert;
    }

    else
    {
        section->end->next = insert;
        section->end = insert;
    }

}

void 
insert_text_at(source_section *section, memory_arena *arena, cc64 text)
{

    u64 string_length = strlen(text) + 1;
    c64 string_buffer = memory_arena_push(arena, string_length);

    memory_copy_simple(string_buffer, text, string_length);

    source_string *insert = memory_arena_push_type(arena, source_string);
    insert->buffer = string_buffer;
    insert->size = string_length - 1;
    insert->next = NULL;

    if (section->start == NULL)
    {
        section->start = insert;
        section->end = insert;
    }

    else
    {
        section->end->next = insert;
        section->end = insert;
    }

}

