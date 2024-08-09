// --- Runtime Environment -----------------------------------------------------
//
// The runtime environment is where the magic happens.
//

#include <state.h>
#include <runtime.h>

#include <platform/fileio.h>
#include <platform/system.h>

#include <core/definitions.h>

#include <compiler/rparser.h>
#include <compiler/stringpool.h>
#include <compiler/symboltable.h>

// --- Environment Initialize --------------------------------------------------
//
// Fetch memory, resources, and other critical initialization procedures that
// must occur before the application begins.
//

int
environment_initialize(i32 argument_count, char ** argument_list)
{

    // Get the parameters.
    runtime_parameters *parameters = &(get_state()->parameters);
    memory_arena *primary_arena = &(get_state()->primary_arena);

    // Load runtime parameters defaults.
    parameters->arg_current          = 1; // Always start at 1.
    parameters->arg_count            = argument_count;
    parameters->arguments            = argument_list;
    parameters->memory_limit         = SF_GIGABYTES(2);
    parameters->string_pool_limit    = SF_MEGABYTES(256);
    parameters->output_directory     = "./";
    parameters->output_name          = "main";

    // Parse and validate all command line arguments.
    b32 command_line_valid = command_line_parse(parameters);
    if (parameters->helped == true)
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
    printf("-- Memory commit:       %lluMB\n", parameters->memory_limit/(1024*1024));
    printf("-- String pool size:    %lluMB\n", parameters->string_pool_limit/(1024*1024));
    printf("-- Compiling:           %s\n", parameters->source_file_path);

    // Establish allocator region.
    void *primary_memory_buffer = system_virtual_alloc(NULL, parameters->memory_limit);
    if (primary_memory_buffer == NULL) return STATUS_CODE_ALLOC_FAIL;
    memory_arena_initialize(primary_arena, primary_memory_buffer, parameters->memory_limit);

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

    // Get the parameters.
    runtime_parameters *parameters = &(get_state()->parameters);
    memory_arena *primary_arena = &(get_state()->primary_arena);

    // --- Unit Testing --------------------------------------------------------
    //
    // If the unit test flag is switched on, we perform all unit tests.
    //

    if (parameters->options.unit_test == true)
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
    
    // --- Symbol Table Testing -----------------------------------------------
    //
    // Symbol table testing, resizing, etc.
    //

#   if 0
        
        symbol_table test_table;
        symbol_table_initialize(&test_table, &primary_arena, SF_MEGABYTES(1));
        symbol *foo_symbol = symbol_table_insert(&test_table, "foo", SYMBOL_TYPE_UNDEFINED);
        symbol *bar_symbol = symbol_table_insert(&test_table, "bar", SYMBOL_TYPE_UNDEFINED);
        symbol *foo_find = symbol_table_search_from_current_table(&test_table, "foo");
        symbol *bar_find = symbol_table_search_from_current_table(&test_table, "bar");
        assert(foo_find != NULL);
        assert(bar_find != NULL);

        b32 contiguous_resize = symbol_table_resize(&test_table);

        memory_arena_push(&primary_arena, 8);
        b32 discontiguous_resize = symbol_table_resize(&test_table);

        memory_arena_pop(&primary_arena, 8);
        b32 reattempt_resize = symbol_table_resize(&test_table);

        assert(contiguous_resize == true);
        assert(discontiguous_resize == false);
        assert(reattempt_resize == true);


#   endif

    //
    //
    //
    // -------------------------------------------------------------------------

    u64 source_size = fileio_file_size(parameters->source_file_path);
    c64 source_buffer = malloc(sizeof(char) * (source_size + 1));
    fileio_file_read(parameters->source_file_path, source_buffer, source_size, source_size + 1);
    source_buffer[source_size] = '\0'; // Null-terminate.

    source_parser parser = {0};
    syntax_node *root = source_parser_create_ast(&parser, source_buffer, 
            parameters->source_file_path, primary_arena);
    if (root != NULL) parser_print_tree(root);
    printf("\n\n");

    printf("-- Arena Stack Size:    %llu bytes\n", primary_arena->size);
    printf("-- Arena Stack Commit:  %llu bytes\n", memory_arena_commit_size(primary_arena));
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
    //if (primary_arena.buffer != NULL) system_virtual_free(primary_arena.buffer);

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

#if 0
    if (primary_arena.buffer == NULL) return false;
    u64 primary_u64 = memory_arena_save(&primary_arena);


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
        u64 memory_copy_state = memory_arena_save(&primary_arena); 
        printf("-- Memory Copy Throughput Testing 1000 Reps @ 16KB\n");
        u64 test_size = SF_KILOBYTES(16);
        unit_test_memory_copy memcpy_params = {0};
        void *source = memory_arena_push(&primary_arena, test_size);
        void *dest = memory_arena_push(&primary_arena, test_size);
        memcpy_params.source = source;
        memcpy_params.dest = dest;
        memcpy_params.size = test_size;

        unit_test_repitition("memory_copy_ext()", 1000, unit_test_repfn_core_memory_copy_ext, &memcpy_params);
        unit_test_repitition("memory_copy_simple()", 1000, unit_test_repfn_core_memory_copy_simple, &memcpy_params);
        unit_test_repitition("c-stdlib memcpy()", 1000, unit_test_repfn_clib_memcpy, &memcpy_params);
        printf("\n");

        memory_arena_restore(&primary_arena, memory_copy_state);
    }

    {
        u64 memory_copy_state = memory_arena_save(&primary_arena); 
        printf("-- Memory Copy Throughput Testing 1000 Reps @ 4MB\n");
        u64 test_size = SF_MEGABYTES(4);
        unit_test_memory_copy memcpy_params = {0};
        void *source = memory_arena_push(&primary_arena, test_size);
        void *dest = memory_arena_push(&primary_arena, test_size);
        memcpy_params.source = source;
        memcpy_params.dest = dest;
        memcpy_params.size = test_size;

        unit_test_repitition("memory_copy_ext()", 1000, unit_test_repfn_core_memory_copy_ext, &memcpy_params);
        unit_test_repitition("memory_copy_simple()", 1000, unit_test_repfn_core_memory_copy_simple, &memcpy_params);
        unit_test_repitition("c-stdlib memcpy()", 1000, unit_test_repfn_clib_memcpy, &memcpy_params);
        printf("\n");

        memory_arena_restore(&primary_arena, memory_copy_state);
    }

    {
        u64 memory_copy_state = memory_arena_save(&primary_arena); 
        printf("-- Memory Copy Throughput Testing 1000 Reps @ 32MB\n");
        u64 test_size = SF_MEGABYTES(32);
        unit_test_memory_copy memcpy_params = {0};
        void *source = memory_arena_push(&primary_arena, test_size);
        void *dest = memory_arena_push(&primary_arena, test_size);
        memcpy_params.source = source;
        memcpy_params.dest = dest;
        memcpy_params.size = test_size;

        unit_test_repitition("memory_copy_ext()", 1000, unit_test_repfn_core_memory_copy_ext, &memcpy_params);
        unit_test_repitition("memory_copy_simple()", 0, unit_test_repfn_core_memory_copy_simple, &memcpy_params);
        unit_test_repitition("c-stdlib memcpy()", 1000, unit_test_repfn_clib_memcpy, &memcpy_params);
        printf("\n");

        memory_arena_restore(&primary_arena, memory_copy_state);
    }

    // Return a valid test code.
    memory_arena_restore(&primary_arena, primary_u64);
    return true;
#endif
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
            printf("--      %32s : %.6fms           \r", name, lowest_interval_time);
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
