#include <runtime.h>

#include <platform/fileio.h>
#include <platform/system.h>

#include <core/utilities.h>
#include <core/definitions.h>

#include <scan.h>

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
    // Converts a source file to its tokenized representation. The scanner produces
    // two linked lists. The firt is a list of valid tokens that the parser collected
    // and a list of tokens that are unrecognized by the parser. 
    //
    // If the scanner returns zero, indicating there are no errors in the error list
    // and the token list contains at least one parsed token in total. Likewise,
    // if the scanner returns a value greater than zero, than the value corresponds
    // to the number of errors it encountered. A value of -1 indicates no tokens
    // were parsed and the source file was either empty or contained trivial whitespace.
    //

    array<token> token_list;
    array<token> error_list;

    bool scan_status = scanner_scan_source_file(source_file.str(), &token_list, &error_list);
    if (!scan_status)
    {

        printf("The scanner encountered errors:\n");
        for (size_t idx = 0; idx < error_list.size(); ++idx)
        {
            printf("    Undefined symbol encountered on line: %lld\n", error_list[idx].line);
        }

    }
    else
    {
        printf("The scanner results:\n");
        for (size_t idx = 0; idx < token_list.size(); ++idx)
        {
            if (token_list[idx].type == token_type::END_OF_FILE) break;
            string token_string = token_to_string(token_list[idx]);
            printf("    Symbol '%s' encountered on line: %lld\n", 
                    token_string.str(), token_list[idx].line);
        }
        
    }

    // --- Cleanup Phase -------------------------------------------------------
    //
    // Transpilation process has ended, clean up resources.
    //

    //memory_free(buffer);
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
