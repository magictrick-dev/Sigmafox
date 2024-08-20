#include <compiler/generation.h>

// --- AST Transpilation Routine -----------------------------------------------

void 
transpile_program_node(syntax_node *root_node, source_file *file, memory_arena *arena)
{

    assert(root_node->type == PROGRAM_ROOT_NODE);

    insert_text_at(file->body, arena, "int\n");
    insert_text_at(file->body, arena, "main(int argc, char **argv)\n");
    insert_text_at(file->body, arena, "{\n");
    push_tabs_at(file->body);
        insert_text_at(file->body, arena, "\n");
        insert_text_at(file->body, arena, "std::string message = \"Hello, world!\";\n");
        insert_text_at(file->body, arena, "std::cout << message << std::endl;\n");
        insert_text_at(file->body, arena, "\n");
    pop_tabs_at(file->body);
    insert_text_at(file->body, arena, "}\n");

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
insert_text_at(source_section *section, memory_arena *arena, cc64 text)
{

    u64 string_length = strlen(text) + 1;
    u64 buffer_length = string_length + section->tab_depth;
    c64 string_buffer = memory_arena_push(arena, buffer_length);

    for (u64 t = 0; t < section->tab_depth; ++t) string_buffer[t] = ' ';
    memory_copy_simple(string_buffer + section->tab_depth, text, string_length);

    source_string *insert = memory_arena_push_type(arena, source_string);
    insert->buffer = string_buffer;
    insert->size = buffer_length - 1;
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

