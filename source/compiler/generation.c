#include <compiler/generation.h>

// --- AST Transpilation Routine -----------------------------------------------

void
transpile_parameter_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == PARAMETER_STATEMENT_NODE);
    insert_text_at(section, arena, "int ");
    insert_text_at(section, arena, root_node->parameter.name);

}

void
transpile_procedure_call_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == PROCEDURE_CALL_EXPRESSION_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, root_node->proc_call.identifier);
    insert_text_at(section, arena, "(");

    syntax_node *params = root_node->proc_call.parameters;
    while(params != NULL)
    {
        transpile_node(params, section, file, arena);
        if (params->next_node != NULL) 
            insert_text_at(section, arena, ", ");
        params = params->next_node;
    }

    insert_text_at(section, arena, ");\n");

}

void
transpile_function_call_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == FUNCTION_CALL_EXPRESSION_NODE);

    insert_text_at(section, arena, "fn_");
    insert_text_at(section, arena, root_node->func_call.identifier);
    insert_text_at(section, arena, "(");

    syntax_node *params = root_node->func_call.parameters;
    while(params != NULL)
    {
        transpile_node(params, section, file, arena);
        if (params->next_node != NULL) 
            insert_text_at(section, arena, ", ");
        params = params->next_node;
    }

    insert_text_at(section, arena, ")");

}

void
transpile_procedure_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == PROCEDURE_STATEMENT_NODE);
    section = file->header;

    insert_text_at(section, arena, "void ");
    insert_text_at(section, arena, root_node->procedure.name);
    insert_text_at(section, arena, "(");

    syntax_node *params = root_node->procedure.parameters;
    while (params != NULL)
    {
        transpile_node(params, section, file, arena);
        if (params->parameter.next_parameter != NULL) 
            insert_text_at(section, arena, ", ");
        params = params->parameter.next_parameter;
    }

    insert_text_at(section, arena, ")\n");
    insert_text_at(section, arena, "{\n\n");
    push_tabs_at(section);

        syntax_node *current_node = root_node->procedure.body_statements;
        while (current_node != NULL)
        {

            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;

        }

    pop_tabs_at(section);
    insert_text_at(section, arena, "\n");
    insert_text_at(section, arena, "}\n\n");

}

void
transpile_function_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == FUNCTION_STATEMENT_NODE);
    section = file->header;

    insert_text_at(section, arena, "int fn_");
    insert_text_at(section, arena, root_node->function.name);
    insert_text_at(section, arena, "(");

    syntax_node *params = root_node->function.parameters;
    while (params != NULL)
    {
        transpile_node(params, section, file, arena);
        if (params->parameter.next_parameter != NULL) 
            insert_text_at(section, arena, ", ");
        params = params->parameter.next_parameter;
    }

    insert_text_at(section, arena, ")\n");
    insert_text_at(section, arena, "{\n\n");
    push_tabs_at(section);

        insert_tabbing_at(section, arena);
        insert_text_at(section, arena, "int ");
        insert_text_at(section, arena, root_node->function.name);
        insert_text_at(section, arena, ";\n\n");

        syntax_node *current_node = root_node->function.body_statements;
        while (current_node != NULL)
        {

            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;

        }

        insert_text_at(section, arena, "\n");
        insert_tabbing_at(section, arena);
        insert_text_at(section, arena, "return ");
        insert_text_at(section, arena, root_node->function.name);
        insert_text_at(section, arena, ";\n");

    pop_tabs_at(section);
    insert_text_at(section, arena, "\n");
    insert_text_at(section, arena, "}\n\n");

}

void
transpile_elseif_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == ELSEIF_STATEMENT_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "else if (");
    transpile_node(root_node->elseif_conditional.evaluation_expression, section, file, arena);
    insert_text_at(section, arena, ")\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "{\n\n");
    push_tabs_at(section);
    
        syntax_node *current_node = root_node->elseif_conditional.body_statements;
        while (current_node != NULL)
        {

            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;

        }

    pop_tabs_at(section);
    insert_text_at(section, arena, "\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "}\n\n");

}

void
transpile_if_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == IF_STATEMENT_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "if (");
    transpile_node(root_node->if_conditional.evaluation_expression, section, file, arena);
    insert_text_at(section, arena, ")\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "{\n\n");
    push_tabs_at(section);

        syntax_node *current_node = root_node->if_conditional.body_statements;
        while (current_node != NULL)
        {

            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;

        }

    pop_tabs_at(section);
    insert_text_at(section, arena, "\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "}\n\n");

    syntax_node *else_node = root_node->if_conditional.else_statement;
    while (else_node != NULL)
    {
        transpile_node(else_node, section, file, arena);
        else_node = else_node->elseif_conditional.else_statement;
    }
}

void 
transpile_read_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == READ_STATEMENT_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "std::cin >> ");
    insert_text_at(section, arena, root_node->read.identifier);
    insert_text_at(section, arena, ";\n");

}

void 
transpile_write_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == WRITE_STATEMENT_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "std::cout << ");
    
    syntax_node *chain = root_node->write.body_expressions;
    while (chain != NULL)
    {

        transpile_node(chain, section, file, arena);   

        chain = chain->next_node;
        if (chain != NULL)
        {
            insert_text_at(section, arena, " << ");
        }

    }

    insert_text_at(section, arena, " << std::endl;\n");

}

void
transpile_loop_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{
    
    assert(root_node->type == LOOP_STATEMENT_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "for (");
    insert_text_at(section, arena, "int ");
    insert_text_at(section, arena, root_node->for_loop.iterator_identifier);
    insert_text_at(section, arena, " = ");
    transpile_node(root_node->for_loop.initial_value_expression, section, file, arena);
    insert_text_at(section, arena, "; ");
    insert_text_at(section, arena, root_node->for_loop.iterator_identifier);
    insert_text_at(section, arena, " < ");
    transpile_node(root_node->for_loop.terminate_value_expression, section, file, arena);
    insert_text_at(section, arena, "; ");
    insert_text_at(section, arena, root_node->for_loop.iterator_identifier);
    insert_text_at(section, arena, " += ");
    if (root_node->for_loop.step_value_expression == NULL)
        insert_text_at(section, arena, "1");
    else
        transpile_node(root_node->for_loop.step_value_expression, section, file, arena);

    insert_text_at(section, arena, ")\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "{\n\n");
    push_tabs_at(section);

        insert_tabbing_at(section, arena);
        insert_text_at(section, arena, "// Cache iterator to restore initial value at end-of-loop.\n");
        insert_tabbing_at(section, arena);
        insert_text_at(section, arena, "int __");
        insert_text_at(section, arena, root_node->for_loop.iterator_identifier);
        insert_text_at(section, arena, "_iter = ");
        insert_text_at(section, arena, root_node->for_loop.iterator_identifier);
        insert_text_at(section, arena, ";\n\n");

        syntax_node *current_node = root_node->for_loop.body_statements;
        while (current_node != NULL)
        {

            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;

        }

        insert_text_at(section, arena, "\n\n");
        insert_tabbing_at(section, arena);
        insert_text_at(section, arena, root_node->for_loop.iterator_identifier);
        insert_text_at(section, arena, " = __");
        insert_text_at(section, arena, root_node->for_loop.iterator_identifier);
        insert_text_at(section, arena, "_iter;\n\n");

    pop_tabs_at(section);
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "};\n\n");
        
}

void
transpile_while_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == WHILE_STATEMENT_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "while (");
    transpile_node(root_node->while_loop.evaluation_expression, section, file, arena);
    insert_text_at(section, arena, ")\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "{\n\n");
    push_tabs_at(section);

        syntax_node *current_node = root_node->while_loop.body_statements;
        while (current_node != NULL)
        {
            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;

        }

    pop_tabs_at(section);
    insert_text_at(section, arena, "\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "};\n\n");

}

void
transpile_scope_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == SCOPE_STATEMENT_NODE);

    insert_text_at(section, arena, "\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "{\n\n");
    push_tabs_at(section);
        
        syntax_node *current_node = root_node->scope.body_statements;
        while(current_node != NULL)
        {
            
            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;

        }

    pop_tabs_at(section);
    insert_text_at(section, arena, "\n");
    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "};\n\n");

}

void 
transpile_program_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == PROGRAM_ROOT_NODE);

    syntax_node *global_node = root_node->program.global_statements;
    while (global_node != NULL)
    {

        transpile_node(global_node, section, file, arena);
        global_node = global_node->next_node;
        
    }

    insert_text_at(section, arena, "int\n");
    insert_text_at(section, arena, "main(int argc, char **argv)\n");
    insert_text_at(section, arena, "{\n\n");
    push_tabs_at(section);

        syntax_node *current_node = root_node->program.body_statements;
        while (current_node != NULL)
        {

            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;

        }

    pop_tabs_at(section);
    insert_text_at(section, arena, "\n}\n");

}

void
transpile_variable_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == VARIABLE_STATEMENT_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, "int ");
    insert_text_at(section, arena, root_node->variable.name);

    if (root_node->variable.dimensions != NULL)
    {
        
        insert_text_at(section, arena, "[");

        syntax_node *current_node = root_node->variable.dimensions;
        while (current_node != NULL)
        {

            transpile_node(current_node, section, file, arena);
            current_node = current_node->next_node;
            if (current_node != NULL) insert_text_at(section, arena, ", ");

        }

        insert_text_at(section, arena, "]");

    }

    else if (root_node->variable.assignment != NULL)
    {
        insert_text_at(section, arena, " = ");
        transpile_node(root_node->variable.assignment, section, file, arena);
    }

    insert_text_at(section, arena, ";\n");

}

void 
transpile_array_index_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    SF_ENSURE_PTR(root_node);   
    SF_ENSURE_PTR(section);   
    SF_ENSURE_PTR(file);   
    SF_ENSURE_PTR(arena);   

    SF_ASSERT(root_node->type == ARRAY_INDEX_EXPRESSION_NODE);

    insert_tabbing_at(section, arena);
    insert_text_at(section, arena, root_node->array_index.name);
    insert_text_at(section, arena, "[");
    
    syntax_node *current_accessor = root_node->array_index.accessors;
    while (current_accessor != NULL)
    {

        transpile_node(current_accessor, section, file, arena);
        current_accessor = current_accessor->next_node;
        if (current_accessor != NULL) insert_text_at(section, arena, ", ");

    }

    insert_text_at(section, arena, "]");

}

void 
transpile_expression_statement_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena)
{

    SF_ENSURE_PTR(root_node);
    SF_ENSURE_PTR(section);
    SF_ENSURE_PTR(file);
    SF_ENSURE_PTR(arena);
    SF_ASSERT(root_node->type == EXPRESSION_STATEMENT_NODE);

    transpile_node(root_node->expression.expression, section, file, arena);
    insert_text_at(section, arena, ";\n");

}


void 
transpile_binary_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    SF_ASSERT(root_node->type == BINARY_EXPRESSION_NODE);

    transpile_node(root_node->binary.left, section, file, arena);

    switch (root_node->binary.type)
    {

        case OPERATION_ADDITION:            insert_text_at(section, arena, " + "); break;
        case OPERATION_SUBTRACTION:         insert_text_at(section, arena, " - "); break;
        case OPERATION_MULTIPLICATION:      insert_text_at(section, arena, " * "); break;
        case OPERATION_DIVISION:            insert_text_at(section, arena, " / "); break;
        case OPERATION_EQUALS:              insert_text_at(section, arena, " == "); break;
        case OPERATION_NOT_EQUALS:          insert_text_at(section, arena, " != "); break;
        case OPERATION_LESS_THAN:           insert_text_at(section, arena, " < "); break;
        case OPERATION_LESS_THAN_EQUALS:    insert_text_at(section, arena, " <= "); break;
        case OPERATION_GREATER_THAN:        insert_text_at(section, arena, " > "); break;
        case OPERATION_GREATER_THAN_EQUALS: insert_text_at(section, arena, " >= "); break;
        case OPERATION_ASSIGNMENT:          insert_text_at(section, arena, " = "); break;
        
        default:
        {

            assert(!"Unimplemented operation for binary expression.");
            return;

        } break;
    }

    transpile_node(root_node->binary.right, section, file, arena);

}

void
transpile_unary_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == UNARY_EXPRESSION_NODE);

    switch (root_node->unary.type)
    {

        case OPERATION_NEGATIVE_ASSOCIATE: insert_text_at(section, arena, "-"); break;

        default:
        {

            assert(!"Unimplemented operation for unary expression printing.");
            return;

        } break;

    };

    transpile_node(root_node->unary.right, section, file, arena);

}

void
transpile_grouping_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == GROUPING_EXPRESSION_NODE);

    insert_text_at(section, arena, "( ");
        transpile_node(root_node->grouping.grouping, section, file, arena);
    insert_text_at(section, arena, " )");

}

void
transpile_assignment_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == ASSIGNMENT_EXPRESSION_NODE);

    insert_tabbing_at(section, arena);
    //transpile_node(root_node->assignment.left, section, file, arena);
    insert_text_at(section, arena, root_node->assignment.identifier);
    insert_text_at(section, arena, " = ");
    transpile_node(root_node->assignment.right, section, file, arena);

}

void
transpile_primary_node(syntax_node *root_node, source_section *section, source_file *file, memory_arena *arena)
{

    assert(root_node->type == PRIMARY_EXPRESSION_NODE);

    if (root_node->primary.type == OBJECT_STRING)
    {
        insert_text_at(section, arena, "\"");
    }

    insert_text_at(section, arena, root_node->primary.literal.identifier);

    if (root_node->primary.type == OBJECT_STRING)
    {
        insert_text_at(section, arena, "\"");
    }

}

void
transpile_node(syntax_node *root_node, source_section *section,  source_file *file, memory_arena *arena)
{

    switch(root_node->type)
    {
        
        case PROGRAM_ROOT_NODE:
        {

            transpile_program_node(root_node, section, file, arena);
            return;

        } break;

        case WRITE_STATEMENT_NODE:
        {

            transpile_write_node(root_node, section, file, arena);
            return;

        } break;

        case READ_STATEMENT_NODE:
        {

            transpile_read_node(root_node, section, file, arena);
            return;

        } break;

        case PROCEDURE_CALL_EXPRESSION_NODE:
        {

            transpile_procedure_call_node(root_node, section, file, arena);
            return;

        } break;

        case FUNCTION_CALL_EXPRESSION_NODE:
        {

            transpile_function_call_node(root_node, section, file, arena);
            return;

        } break;

        case PROCEDURE_STATEMENT_NODE:
        {

            transpile_procedure_node(root_node, section, file, arena);
            return;

        } break;

        case FUNCTION_STATEMENT_NODE:
        {

            transpile_function_node(root_node, section, file, arena);
            return;

        } break;

        case PARAMETER_STATEMENT_NODE:
        {

            transpile_parameter_node(root_node, section, file, arena);
            return;

        } break;

        case ARRAY_INDEX_EXPRESSION_NODE:
        {

            transpile_array_index_node(root_node, section, file, arena);
            return;

        } break;

        case IF_STATEMENT_NODE:
        {
            transpile_if_node(root_node, section, file, arena);
            return;
        } break;

        case ELSEIF_STATEMENT_NODE:
        {
            transpile_elseif_node(root_node, section, file, arena);
            return;
        } break;

        case LOOP_STATEMENT_NODE:
        {
            transpile_loop_node(root_node, section, file, arena);
            return;
        } break;

        case WHILE_STATEMENT_NODE:
        {
            transpile_while_node(root_node, section, file, arena);
            return;
        } break;

        case SCOPE_STATEMENT_NODE:
        {
            transpile_scope_node(root_node, section, file, arena);
            return; 
        } break;

        case VARIABLE_STATEMENT_NODE:
        {
            transpile_variable_node(root_node, section, file, arena);
            return;
        } break;

        case EXPRESSION_STATEMENT_NODE:
        {
            transpile_expression_statement_node(root_node, section, file, arena);
            return;
        } break;

        case BINARY_EXPRESSION_NODE:
        {
            transpile_binary_node(root_node, section, file, arena);
            return;
        } break;

        case UNARY_EXPRESSION_NODE:
        {
            transpile_unary_node(root_node, section, file, arena);
            return;
        } break;

        case PRIMARY_EXPRESSION_NODE:
        {
            transpile_primary_node(root_node, section, file, arena);
            return;
        }

        case ASSIGNMENT_EXPRESSION_NODE:
        {
            transpile_assignment_node(root_node, section, file, arena);
            return;
        } return;

        case GROUPING_EXPRESSION_NODE:
        {
            transpile_grouping_node(root_node, section, file, arena);
            return;
        } break;

        default:
        {
            assert(!"Uncaught transpilation case for syntax node.");
            return;
        }

    };

}

b32
transpile_syntax_tree(syntax_node *root_node, memory_arena *arena, cc64 output_name)
{

    assert(root_node != NULL);

    source_file cmake_file = {0};
    cmake_file.file_name = "./output/CMakeLists.txt";
    cmake_file.header = memory_arena_push_type(arena, source_section);
    cmake_file.body = memory_arena_push_type(arena, source_section);
    cmake_file.footer = memory_arena_push_type(arena, source_section);

    cmake_file.header->start     = NULL;
    cmake_file.header->end       = NULL;
    cmake_file.header->tab_depth = 0;

    cmake_file.body->start       = NULL;
    cmake_file.body->end         = NULL;
    cmake_file.body->tab_depth   = 0;

    cmake_file.footer->start     = NULL;
    cmake_file.footer->end       = NULL;
    cmake_file.footer->tab_depth = 0;

    insert_text_at(cmake_file.header, arena, "CMAKE_MINIMUM_REQUIRED(VERSION 3.21)\n\n");
    insert_text_at(cmake_file.header, arena, "PROJECT(sigmafox_build)\n\n");
    insert_text_at(cmake_file.header, arena, "ADD_EXECUTABLE(sigmafox_build\n");
    insert_text_at(cmake_file.footer, arena, ")\n\n");

    source_file main_file = {0};
    main_file.file_name = "./output/main.cpp";
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
    insert_text_at(main_file.header, arena, "// Generated C++ using Sigmafox Ver. 0.3.0A\n");
    insert_text_at(main_file.header, arena, "// Northern Illinois University\n");
    insert_text_at(main_file.header, arena, "#include <iostream>\n");
    insert_text_at(main_file.header, arena, "#include <string>\n");
    insert_text_at(main_file.header, arena, "\n\n");

    // Traverse our program nodes.
    transpile_program_node(root_node, main_file.body, &main_file, arena);

    // Add the main file.
    insert_text_at(cmake_file.body, arena, "    \"main.cpp\"\n");

    // Write the transpiled output to disk.
    b32 cmake_file_write_status = write_generated_output_to_disk(&cmake_file);
    b32 main_file_write_status = write_generated_output_to_disk(&main_file);

    return (cmake_file_write_status && main_file_write_status);

}

// --- Helpers -----------------------------------------------------------------

b32
write_generated_output_to_disk(source_file *file)
{

    void* write_handle = fileio_write_stream_open(file->file_name);
    if (write_handle == NULL)
    {
        printf("-- Unable to write transpiled output to %s: unable to open file.\n",
                file->file_name);
        return false;
    }

    source_string *head_section = file->header->start;
    while (head_section != NULL)
    {
        fileio_write_stream_write(write_handle, head_section->buffer, head_section->size);
        head_section = head_section->next;
    }

    source_string *body_section = file->body->start;
    while (body_section != NULL)
    {
        fileio_write_stream_write(write_handle, body_section->buffer, body_section->size);
        body_section = body_section->next;
    }

    source_string *footer_section = file->footer->start;
    while (footer_section != NULL)
    {
        fileio_write_stream_write(write_handle, footer_section->buffer, footer_section->size);
        footer_section = footer_section->next;
    }

    fileio_write_stream_close(write_handle);

    return true;

}

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

