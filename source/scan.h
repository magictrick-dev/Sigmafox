// --- Scanner -----------------------------------------------------------------
// 
// The scanner is designed to assist in the tokenization of source files. It's a
// fast, single operation step that generates a list of tokens based on some
// source file string.
//

#ifndef SIGMAFOX_SCAN_H
#define SIGMAFOX_SCAN_H
#include <core/utilities.h>
#include <core/definitions.h>
#include <token.h>

bool scanner_scan_source_file(const char *source, array<token> *tokens, array<token> *errors);

#endif
