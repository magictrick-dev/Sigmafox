#ifndef SIGMAFOX_RUNTIME_H
#define SIGMAFOX_RUNTIME_H
#include <cstdio>

#define STATUS_CODE_SUCCESS         0
#define STATUS_CODE_NO_ARGS         1
#define STATUS_CODE_NO_FILE         2
#define STATUS_CODE_ALLOC_FAIL      3

int     environment_initialize(int argument_count, char ** argument_list);
int     environment_runtime();
void    environment_shutdown(int status_code);

#endif
