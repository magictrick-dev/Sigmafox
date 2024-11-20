#ifndef SOURCE_COMPILER_GENERATION_H
#define SOURCE_COMPILER_GENERATION_H
#include <core/arena.h>
#include <core/definitions.h>
#include <platform/fileio.h>
#include <compiler/parser.h>

#define TAB_SPACE_SIZE 4

typedef struct source_string source_string;
typedef struct source_section source_section;
typedef struct source_file source_file;

typedef struct source_string
{
    c64 buffer;
    u64 size;
    source_string *next;
} source_string;

typedef struct source_section
{
    source_string *start;
    source_string *end;
    u64 tab_depth;
} source_section;

typedef struct source_file
{
    cc64 file_name;
    source_section *header;
    source_section *body;
    source_section *footer;

    source_file *next_source;
} source_file;

typedef struct generator_state
{
    source_file    *cmake_file;
    source_file    *source_file_list;
    memory_arena   *arena;
} generator_state;

// --- Traversals --------------------------------------------------------------

void transpile_binary_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_unary_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_grouping_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_scope_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_loop_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_while_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_write_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_read_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_if_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_elseif_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_variable_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_array_index_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_expression_statement_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_procedure_call_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_function_call_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_procedure_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_function_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_program_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
void transpile_node(syntax_node *root_node, source_section *section,
        source_file *file, memory_arena *arena);
b32 transpile_syntax_tree(syntax_node *root_node, memory_arena *arena,
        cc64 output_name);

// --- Helpers -----------------------------------------------------------------

void push_tabs_at(source_section *section);
void pop_tabs_at(source_section *section);
void insert_tabbing_at(source_section *section, memory_arena *arena);
void insert_text_at(source_section *section, memory_arena *arena, cc64 text);
b32 write_generated_output_to_disk(source_file *file);

#endif
