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
    array<string> source_files;
} environment_state;

static environment_state *state = NULL;

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
    state = new (memory_alloc_type(environment_state)) environment_state;
    if (state == NULL) return STATUS_CODE_ALLOC_FAIL;

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

        state->source_files.push({argument_list[idx]});

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
    size_t source_size = fileio_file_size(state->source_files[0].str());
    string source_file(source_size + 1);
    fileio_file_read(state->source_files[0].str(), source_file.buffer(), 
            source_size, source_size + 1);

    // --- Scanning Phase ------------------------------------------------------
    //
    // Parses and tokenizes the source file. An error list is produced when the
    // function returns false, indicating that it failed (at least one token is
    // not recognized).
    //

    array<token> token_list;
    array<token> error_list;

    bool scan_status = parser_tokenize_source_file(source_file.str(), &token_list, &error_list);
    if (!scan_status)
    {

        for (size_t idx = 0; idx < error_list.size(); ++idx)
            print_symbol_error(state->source_files[0], &error_list[idx]);

    }
    else
    {
        for (size_t idx = 0; idx < token_list.size(); ++idx)
        {
            if (token_list[idx].type == token_type::END_OF_FILE) break;
            print_symbol(state->source_files[0], &token_list[idx]);
        }
    }

    // --- Cleanup Phase -------------------------------------------------------
    //
    // Transpilation process has ended, clean up resources.
    //

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
    if (state != NULL) memory_free(state);

}
