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

#include <core/definitions.h>
#include <core/arena.h>

#include <compiler/rparser.h>

static cc64 source_file;
static memory_arena primary_arena;
#define SF_PRIMARY_STORE_SIZE SF_GIGABYTES(2)

// --- Environment Initialize --------------------------------------------------
//
// Fetch memory, resources, and other critical initialization procedures that
// must occur before the application begins.
//

int
environment_initialize(i32 argument_count, char ** argument_list)
{

    // Initialize environment state first. We need to use placement new here
    // because our tracked allocator doesn't manually invoke and cascade constructors
    // for us, hence the strange "new" syntax below.
/*
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
*/

    // NOTE(Chris): For now, we aren't doing any real checks here, just assuming
    //              file is valid.
    if (argument_count < 2) return STATUS_CODE_NO_ARGS;
    source_file = argument_list[1];

    // Establish allocator region.
    void *primary_memory_buffer = system_virtual_alloc(NULL, SF_PRIMARY_STORE_SIZE);
    if (primary_memory_buffer == NULL) return STATUS_CODE_ALLOC_FAIL;
    primary_arena.buffer    = primary_memory_buffer;
    primary_arena.size      = SF_PRIMARY_STORE_SIZE;
    primary_arena.commit    = 0;

    return STATUS_CODE_SUCCESS;

}

// --- Environment Runtime -----------------------------------------------------
//
// The residing location for the implementation details of the application. Anything
// within environment runtime should fail softly; edge-case failures must either
// assert during debug development or be handled gracefully.
//

i32
environment_runtime()
{

    u64 source_size = fileio_file_size(source_file);
    c64 source_buffer = malloc(sizeof(char) * (source_size + 1));
    fileio_file_read(source_file, source_buffer, source_size, source_size + 1);
    source_buffer[source_size] = '\0'; // Null-terminate.

    source_parser parser = {0};
    syntax_node *root = source_parser_create_ast(&parser, source_buffer, 
            source_file, &primary_arena);

    return STATUS_CODE_SUCCESS;

}

// --- Environment Shutdown ----------------------------------------------------
//
// When the application is about to shutdown, release resources, memory, etc.
// before finally exitting.
//

void
environment_shutdown(i32 status_code)
{

    system_virtual_free(primary_arena.buffer);

}