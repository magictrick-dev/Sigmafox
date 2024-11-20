#include <compiler/errorhandler.h>

cc64 
friendly_error_string(parser_interrupt interrupt)
{

    switch (interrupt)
    {

        case PARSER_ERROR_FILE_NOEXIST:
        {
            return "file doesn't exist";
        } break;

        case SYSTEM_ERROR_MEMORY_CONSTRAINT_REACHED_STRING_POOL:
        {
            return "reached reserve capacity for string pool";
        } break;

        case SYSTEM_ERROR_MEMORY_CONSTRAINT_REACHED_SYMBOL_TABLE:
        {
            return "reached reserve capacity for symbol table";
        } break;

        case SYSTEM_ERROR_MEMORY_CONSTRAINT_REACHED_ARENA:
        {
            return "reached reserve capacity in global allocator";
        } break;

        case SYSTEM_ERROR_SYMBOL_SHOULD_BE_LOCATABLE:
        {
            return "defined symbol should be locatable";
        } break;

        case PARSER_ERROR_UNEXPECTED_SYMBOL:
        {
            return "unexpected symbol in expression";
        } break;

        case PARSER_ERROR_UNEXPECTED_EOL:
        {
            return "unexpected end of line";
        } break;

        case PARSER_ERROR_UNEXPECTED_EOF:
        {
            return "unexpected end of file";
        } break;

        case PARSER_ERROR_UNEXPECTED_GLOBAL_STATEMENT:
        {
            return "unexpected statement in global scope";
        } break;

        case PARSER_ERROR_EXPECTED_SEMICOLON:
        {
            return "expected semicolon";
        } break;

        case PARSER_ERROR_EXPECTED_SYMBOL:
        {
            return "expected symbol";
        } break;

        case PARSER_ERROR_EXPECTED_IDENTIFIER:
        {
            return "expected identifier";
        } break;

        case PARSER_ERROR_UNDECLARED_IDENTIFIER:
        {
            return "undeclared identifier encountered in expression";
        } break;

        case PARSER_ERROR_UNDEFINED_IDENTIFIER:
        {
            return "undefined identifier encountered in expression";
        } break;

        case PARSER_ERROR_ARITY_MISMATCH:
        {
            return "function/procedure arguments do not match definition argument count";
        } break;

        case PARSER_ERROR_NO_RETURN:
        {
            return "function does not have a return statement";
        } break;

        case PARSER_ERROR_VARIABLE_REDECLARATION:
        {
            return "variable is already defined in scope";
        } break;

        case PARSER_WARNING_VARIABLE_SCOPE_SHADOW:
        {
            return "variable hides a variable of the same in a parent scope";
        } break;

        default:
        {
            assert(!"Undefined error message encountered.");
            return "";
        } break;

    }

}

void 
error_out(u64 source_line, cc64 where, source_token *reference_location, 
        parser_interrupt type, cc64 extended_message, ...)
{

    source_token *error_at = reference_location;
    cc64 file_name = where;

    i32 line;
    i32 column;

    source_token_position(error_at, &line, &column);

    cc64 message = friendly_error_string(type);

    printf("%s (%d,%d) (error:%d:%llu): %s",
            file_name, line, column, type, source_line, message);

    va_list argptr;
    va_start(argptr, extended_message);
    vprintf(extended_message, argptr);
    printf("\n");
    va_end(argptr);

}

void
warning_out(u64 source_line, cc64 where, source_token *reference_location,
        parser_interrupt type, cc64 extended_message, ...)
{

    source_token *error_at = reference_location;
    cc64 file_name = where;

    i32 line;
    i32 column;

    source_token_position(error_at, &line, &column);

    cc64 message = friendly_error_string(type);

    printf("%s (%d,%d) (warning:%d:%llu): %s",
            file_name, line, column, type, source_line, message);

    va_list argptr;
    va_start(argptr, extended_message);
    vprintf(extended_message, argptr);
    printf("\n");
    va_end(argptr);

}

