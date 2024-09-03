#ifndef SOURCE_COMPILER_ERRORHANDLER_H
#define SOURCE_COMPILER_ERRORHANDLER_H
#include <compiler/tokenizer.h>

typedef enum parser_interrupt
{

    SYSTEM_ERROR_MEMORY_CONSTRAINT_REACHED_STRING_POOL,
    SYSTEM_ERROR_MEMORY_CONSTRAINT_REACHED_SYMBOL_TABLE,
    SYSTEM_ERROR_MEMORY_CONSTRAINT_REACHED_ARENA,
    SYSTEM_ERROR_SYMBOL_SHOULD_BE_LOCATABLE,

    PARSER_ERROR_UNEXPECTED_SYMBOL,
    PARSER_ERROR_UNEXPECTED_EOL,
    PARSER_ERROR_UNEXPECTED_EOF,
    PARSER_ERROR_EXPECTED_SEMICLON,
    PARSER_ERROR_EXPECTED_SYMBOL,
    PARSER_ERROR_EXPECTED_IDENTIFIER,
    PARSER_ERROR_ARITY_MISMATCH,
    PARSER_ERROR_NO_RETURN,
    PARSER_ERROR_VARIABLE_REDECLARATION,

} parser_interrupt;

void error_out(u64 source_line, source_token *reference_location, 
        parser_interrupt type, cc64 extended_message, ...);
void warning_out(u64 source_line, source_token *reference_location,
        parser_interrupt type, cc64 extended_message, ...);

#define display_error_message(reference, type, msg, ...) \
    error_out(__LINE__, reference, type, msg, __VA_ARGS__)
#define display_warning_message(reference, type, msg, ...) \
    error_out(__LINE__, reference, type, msg, __VA_ARGS__)

#endif
