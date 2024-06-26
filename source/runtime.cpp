// --- Runtime Environment -----------------------------------------------------
//
// The runtime environment is where the magic happens.
//
// TODO(Chris): In no particular order...
//
//      1.  Decouple any C++ language dependencies to C. This means any placement
//          new operators need to be removed in favor of good ol' fashioned memory
//          management routines. Much of this is already abstracted away.
//
//      2.  Implement an efficient hashing routine as well as a C-style hashmap
//          so that we can speed up our symbol-table searches.
//
//      3.  A C-style string utility library to make it easier to generate and copy
//          strings out to memory.
//
//      4.  Memory copy and memory set routines. This will be important to have
//          towards the file-generation process since we're basically doing large
//          quantities of memory copies.
//
//      5.  A platform multithreading library. This will help in a number of areas
//          to improve performance where parallelism is possible. The most obvious
//          area to consider is the tokenization routine which can easily parallized
//          based on the number of files submitted at compile time.
//

#include <runtime.h>

#include <platform/fileio.h>
#include <platform/system.h>

#include <core/utilities.h>
#include <core/definitions.h>

#include <compiler/parser.h>
#include <compiler/printing.h>

typedef struct
environment_state
{
    array<const char*>  source_files;
    memory_arena        primary_store;
} environment_state;

static environment_state *state = NULL;
#define SF_PRIMARY_STORE_SIZE SF_GIGABYTES(2)

// --- Environment Initialize --------------------------------------------------
//
// Fetch memory, resources, and other critical initialization procedures that
// must occur before the application begins.
//

int
environment_initialize(int argument_count, char ** argument_list)
{

    // Initialize environment state first. We need to use placement new here
    // because our tracked allocator doesn't manually invoke and cascade constructors
    // for us, hence the strange "new" syntax below.
    state = new (memory_allocate_type(environment_state)) environment_state;
    if (state == NULL) return STATUS_CODE_ALLOC_FAIL;

    // Allocate our primary store, ensure allocation success and then inspect
    // the return size to determine upper-bound rounding.
    void *primary_virtual_buffer = system_virtual_alloc(NULL, SF_PRIMARY_STORE_SIZE);
    if (primary_virtual_buffer == NULL) return STATUS_CODE_ALLOC_FAIL;
    state->primary_store.buffer = primary_virtual_buffer;
    state->primary_store.commit = 0;
    state->primary_store.size = system_virtual_buffer_size(primary_virtual_buffer);
    assert(state->primary_store.size >= SF_PRIMARY_STORE_SIZE);

    // Verify the argument list.
    if (argument_count <= 1) return STATUS_CODE_NO_ARGS;

    // Process argument list.
    for (size_t idx = 1; idx < argument_count; ++idx)
    {
        
        bool file_exists = fileio_file_exists(argument_list[idx]);
        if (!file_exists)
        {
            printf("Error, file does not exists: %s\n", argument_list[idx]);
            return STATUS_CODE_NO_FILE;
        }

        state->source_files.push(argument_list[idx]);

    }

    return STATUS_CODE_SUCCESS;

}

// --- Environment Runtime -----------------------------------------------------
//
// The residing location for the implementation details of the application. Anything
// within environment runtime should fail softly; edge-case failures must either
// assert during debug development or be handled gracefully.
//

int
environment_runtime()
{

    // Ensure that state is valid when we reach runtime.
    assert(state != NULL);

    // Pull the first source file into memory.
    size_t source_size = fileio_file_size(state->source_files[0]);
    char *source_buffer = memory_allocate_array(char, source_size + 1);
    fileio_file_read(state->source_files[0], source_buffer, source_size, source_size + 1);
    source_buffer[source_size] = '\0'; // Null-terminate.
    // TODO(Chris): Since decoupling from C++, we're dealing with raw c-strings
    //              once again. This can be a bit error prone (which prompted this
    //              comment). We will need to write a helper library to ensure that
    //              we have better interfacing with strings in a C-style manner.

    // --- Scanning Phase ------------------------------------------------------
    //
    // Parses and tokenizes the source file. An error list is produced when the
    // function returns false, indicating that it failed (at least one token is
    // not recognized).
    //

    array<token> token_list;
    array<token> error_list;

    bool scan_status = parser_tokenize_source_file(state->source_files[0], 
            source_buffer, &token_list, &error_list);
    if (!scan_status)
    {

        printf("Unable to tokenize source file.\n");
        return STATUS_CODE_SUCCESS;

    }

#if 0
    // We will print the list of tokens our for now, but this is temporary.
    printf("------------------------------------------------------------\n");
    printf("                Tokenization Results\n");
    printf("------------------------------------------------------------\n");
    for (size_t idx = 0; idx < token_list.size(); ++idx)
    {
        if (token_list[idx].type == token_type::END_OF_FILE) break;
        print_symbol(state->source_files[0], &token_list[idx]);
    }
#endif

    // --- AST Phase -----------------------------------------------------------
    //
    // Converts the list of tokens to a traversable abstract syntax tree.
    //
#if 1
    printf("------------------------------------------------------------\n");
    printf("         Parser to Transpilation Print Results\n");
    printf("------------------------------------------------------------\n");
    array<statement*> program;   
    bool parser_valid = parser_generate_abstract_syntax_tree(&token_list,
            &program, &state->primary_store);
    if (parser_valid)
    {
        parser_ast_traversal_print(&program);
    }
    else
    {
        return STATUS_CODE_SUCCESS;
    }
#endif

    // --- Cleanup Phase -------------------------------------------------------
    //
    // Transpilation process has ended, clean up resources.
    //

    // Frees the AST from memory.
    //parser_ast_free_traversal(ast_root);
    memory_release(source_buffer);

    return STATUS_CODE_SUCCESS;

}

// --- Environment Shutdown ----------------------------------------------------
//
// When the application is about to shutdown, release resources, memory, etc.
// before finally exitting.
//

void
environment_shutdown(int status_code)
{

    // Free the environment state. Since environment state may contain C++
    // datatypes, we need manually invoke the destructor on the structure.
    state->~environment_state();
    if (state != NULL) memory_release(state);

}
