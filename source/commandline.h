#ifndef SIGMAFOX_COMMANDLINE_H
#define SIGMAFOX_COMMANDLINE_H
#include <core.h>
#include <cstdio>

#define SF_CLOPTYPE_LITERAL 0
#define SF_CLOPTYPE_FLAG    1
#define SF_CLOPTYPE_SWITCH  2

void sigmafox_cli_print_header();
void sigmafox_cli_print_description();
void sigmafox_cli_print_useage();
void sigmafox_cli_print_error(const char *error_message);

#endif
