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
void environment_shutdown(i32 status_code);

#endif
