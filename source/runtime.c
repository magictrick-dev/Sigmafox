// --- Runtime Environment -----------------------------------------------------
//
// The runtime environment is where the magic happens.
//

#include <runtime.h>

#include <platform/fileio.h>
#include <platform/system.h>

#include <core/definitions.h>
#include <core/arena.h>
#include <core/cli.h>

#include <compiler/stringpool.h>
#include <compiler/rparser.h>

static cc64 source_file;
static memory_arena primary_arena;
#define SF_PRIMARY_STORE_SIZE SF_GIGABYTES(2)
static runtime_parameters params;

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

    // Load runtime parameters from command line as well as set defaults.
    params.arg_current          = 1; // Always start at 1.
    params.arg_count            = argument_count;
    params.arguments            = argument_list;
    params.memory_limit         = SF_PRIMARY_STORE_SIZE;
    params.string_pool_limit    = SF_MEGABYTES(256);
    params.output_directory     = "./";
    params.output_name          = "main";

    // Parse and validate.
    b32 command_line_valid = command_line_parse(&params);
    if (params.helped == true)
    {
        printf("-- Transpiler will not run when help flag is enabled.\n");
        return STATUS_CODE_HELP;
    }
    if (command_line_valid == false)
    {
        printf("-- Unable to run transpiler, check command line arguments.\n");
        return STATUS_CODE_BAD_ARGS;
    }

    // Set source file path, as checked.
    source_file = params.source_file_path;
    printf("-- Memory commit:       %lluMB\n", params.memory_limit/(1024*1024));
    printf("-- String pool size:    %lluMB\n", params.string_pool_limit/(1024*1024));
    printf("-- Compiling:           %s\n", source_file);

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

    // --- Unit Testing --------------------------------------------------------
    //
    // If the unit test flag is switched on, we perform all unit tests.
    //

    if (params.options.unit_test == true)
    {
        i32 result = environment_tests();
        if (result == false)
        {
            printf("-- Unit tests have failed, transpilation will not continue.\n");
            return STATUS_CODE_SUCCESS;
        }
    }

    printf("\n");

    // --- Pool Testing --------------------------------------------------------
    //
    // In order to ensure consistency in the API, here we test the string pool
    // hasing routine and resolution.
    //

#   if 0
        string_pool test_pool;
        string_pool_initialize(&test_pool, &primary_arena, SF_MEGABYTES(64));
        sh64 a = string_pool_insert(&test_pool, "Hello");
        sh64 b = string_pool_insert(&test_pool, "World");
        sh64 c = string_pool_insert(&test_pool, "Foo");
        sh64 d = string_pool_insert(&test_pool, "Bar");
        sh64 e = string_pool_insert(&test_pool, "Hello");
        sh64 f = string_pool_insert(&test_pool, "World");
#   endif

    //
    //
    //
    // -------------------------------------------------------------------------

    u64 source_size = fileio_file_size(source_file);
    c64 source_buffer = malloc(sizeof(char) * (source_size + 1));
    fileio_file_read(source_file, source_buffer, source_size, source_size + 1);
    source_buffer[source_size] = '\0'; // Null-terminate.

    source_parser parser = {0};
    syntax_node *root = source_parser_create_ast(&parser, source_buffer, 
            source_file, &primary_arena);
    if (root != NULL) parser_print_tree(root);
    printf("\n\n");

    printf("-- Arena Stack Size:    %llu bytes\n", primary_arena.size);
    printf("-- Arena Stack Commit:  %llu bytes\n", primary_arena.commit);
    if (root != NULL)
        printf("Transpilation was successful.\n");
    else
        printf("Transpilation failed. See errors for more information.\n");

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

    // Free the virtual memory if it was allocated.
    if (primary_arena.buffer != NULL) system_virtual_free(primary_arena.buffer);

}

// --- Environment Tests -------------------------------------------------------
//
// A set of environment tests that can be checked and validated at runtime.
// The following code is not for the faint of heart, it is for software validation
// and performance testing.
//

i32
environment_tests()
{

    if (primary_arena.buffer == NULL) return false;
    arena_state primary_arena_state = memory_arena_cache_state(&primary_arena);


    printf("\n");
    printf("-- Performing unit tests:\n");

    // Get CPU frequency into cache. This is a semi-expensive process and we
    // should perform this routine first. Report the cpu frequency back to the user.
    u64 cpu_frequency = system_cpustamp_frequency();

    // --- Memory Copy ---------------------------------------------------------
    //
    // Testing the speed and throughput of c-standard library memory copy versus
    // the custom memory copy routine. This is a raw through-put routine, not
    // validating copy integrity. Memory copy is undefined for overlapped, therefore
    // integrity checking is not required.
    //

    {
        arena_state memory_copy_state = memory_arena_cache_state(&primary_arena); 
        printf("-- Memory Copy Throughput Testing @ 16KB\n");
        u64 test_size = SF_KILOBYTES(16);
        unit_test_memory_copy memcpy_params = {0};
        void *source = memory_arena_push(&primary_arena, test_size);
        void *dest = memory_arena_push(&primary_arena, test_size);
        memcpy_params.source = source;
        memcpy_params.dest = dest;
        memcpy_params.size = test_size;

        unit_test_repitition("memory_copy_simple()", 1000, unit_test_repfn_core_memory_copy_simple, &memcpy_params);
        unit_test_repitition("c-stdlib memcpy()", 1000, unit_test_repfn_clib_memcpy, &memcpy_params);
        unit_test_repitition("memory_copy_ext()", 1000, unit_test_repfn_core_memory_copy_ext, &memcpy_params);
        printf("\n");

        memory_arena_restore_state(&primary_arena, memory_copy_state);
    }

    {
        arena_state memory_copy_state = memory_arena_cache_state(&primary_arena); 
        printf("-- Memory Copy Throughput Testing @ 4MB\n");
        u64 test_size = SF_MEGABYTES(4);
        unit_test_memory_copy memcpy_params = {0};
        void *source = memory_arena_push(&primary_arena, test_size);
        void *dest = memory_arena_push(&primary_arena, test_size);
        memcpy_params.source = source;
        memcpy_params.dest = dest;
        memcpy_params.size = test_size;

        unit_test_repitition("memory_copy_simple()", 1000, unit_test_repfn_core_memory_copy_simple, &memcpy_params);
        unit_test_repitition("c-stdlib memcpy()", 1000, unit_test_repfn_clib_memcpy, &memcpy_params);
        unit_test_repitition("memory_copy_ext()", 1000, unit_test_repfn_core_memory_copy_ext, &memcpy_params);
        printf("\n");

        memory_arena_restore_state(&primary_arena, memory_copy_state);
    }

    {
        arena_state memory_copy_state = memory_arena_cache_state(&primary_arena); 
        printf("-- Memory Copy Throughput Testing @ 32MB\n");
        u64 test_size = SF_MEGABYTES(32);
        unit_test_memory_copy memcpy_params = {0};
        void *source = memory_arena_push(&primary_arena, test_size);
        void *dest = memory_arena_push(&primary_arena, test_size);
        memcpy_params.source = source;
        memcpy_params.dest = dest;
        memcpy_params.size = test_size;

        unit_test_repitition("memory_copy_simple()", 0, unit_test_repfn_core_memory_copy_simple, &memcpy_params);
        unit_test_repitition("c-stdlib memcpy()", 1000, unit_test_repfn_clib_memcpy, &memcpy_params);
        unit_test_repitition("memory_copy_ext()", 1000, unit_test_repfn_core_memory_copy_ext, &memcpy_params);
        printf("\n");

        memory_arena_restore_state(&primary_arena, memory_copy_state);
    }

    // Return a valid test code.
    memory_arena_restore_state(&primary_arena, primary_arena_state);
    return true;

}

void
unit_test_repitition(cc64 name, u64 count, repetition_routine_fptr repfn, void *user)
{

    if (count == 0)
    {
        printf("--      %32s : Skipped\n", name);
        return;
    }

    r64 last_interval_time = 0;
    r64 lowest_interval_time = 100000000;
    u64 idx = 0;
    u64 freq = system_cpustamp_frequency();
    while (idx < count)
    {

        u64 start = system_cpustamp();
        repfn(user);
        u64 end = system_cpustamp();

        last_interval_time = (r64)(end - start) / freq * 1000;
        if (last_interval_time < lowest_interval_time)
        {
            lowest_interval_time = last_interval_time;
            printf("--      %32s : %.9fms\r", name, lowest_interval_time);
            idx = 0; // Reset on new low.
        }

        idx++;

    }

    printf("\n");

}

void 
unit_test_repfn_clib_memcpy(void *copy_params)
{
    unit_test_memory_copy *cparams = (unit_test_memory_copy*)copy_params;
    memcpy(cparams->dest, cparams->source, cparams->size);
}

void
unit_test_repfn_core_memory_copy_simple(void *copy_params)
{
    unit_test_memory_copy *cparams = (unit_test_memory_copy*)copy_params;
    memory_copy_simple(cparams->dest, cparams->source, cparams->size);
}

void
unit_test_repfn_core_memory_copy_ext(void *copy_params)
{
    unit_test_memory_copy *cparams = (unit_test_memory_copy*)copy_params;
    memory_copy_ext(cparams->dest, cparams->source, cparams->size);
}
