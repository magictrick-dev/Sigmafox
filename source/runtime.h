#ifndef SIGMAFOX_RUNTIME_H
#define SIGMAFOX_RUNTIME_H
#include <core/definitions.h>

#define STATUS_CODE_SUCCESS         0
#define STATUS_CODE_NO_ARGS         1
#define STATUS_CODE_NO_FILE         2
#define STATUS_CODE_ALLOC_FAIL      3
#define STATUS_CODE_BAD_ARGS        4
#define STATUS_CODE_HELP            5

i32 environment_initialize(i32 argument_count, char ** argument_list);
i32 environment_runtime();
i32 environment_tests();
void environment_shutdown(i32 status_code);

typedef void (*repetition_routine_fptr)(void* user);
void unit_test_repitition(cc64 name, u64 count, repetition_routine_fptr repfn, void *user);

// --- Unit Tests: Memory Copying ----------------------------------------------
//
// Ensures that memory copy speeds succeed c-standard library's speed.
//

typedef struct unit_test_memory_copy 
{
    void *source;
    void *dest;
    u64 size;
} unit_test_memory_copy;

void unit_test_repfn_clib_memcpy(void *copy_params);
void unit_test_repfn_core_memory_copy_simple(void *copy_params);
void unit_test_repfn_core_memory_copy_ext(void *copy_params);

#endif
