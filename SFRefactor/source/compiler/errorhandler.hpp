// --- Sigmafox Compiler Error Handler -----------------------------------------
//
// Written by Chris DeJong Nov. 2024 / Northern Illinois University
//
//      The error handler routine is designed to offer a cohesive interface
//      method for displaying human-readable errors to the user. Error messages
//      should provide a where and how, as well as a potential suggestion for
//      fixing the issue.
//
//      The error handler is a static interface. It stores errors messages
//      interally in a buffer so that it can potential dump it to a log file.
//      Eventually, if we want to use multithreading, the error handler will
//      need some synchronization primitives so that there isn't any write
//      contentions between threads.
//
//      The error messages contain additional meta data that can be used for
//      further analysis if the user chooses to dump it to a file.
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_COMPILER_ERROR_HANDLER_HPP
#define SIGMAFOX_COMPILER_ERROR_HANDLER_HPP
#include <definitions.hpp>
#include <vector>
#include <platform/system.hpp>
#include <compiler/tokenizer.hpp>

enum class ErrorType
{
    ErrorTypeGeneralError,
    ErrorTypeGeneralWarning,
    ErrorTypeParserError,
    ErrorTypeParserWarning,
};

struct ErrorMessageFormat 
{
    std::string filepath;
    std::string message;
    std::string offender;
    i32 column_location;
    i32 line_location;
    u64 timestamp;
};

class ErrorHandler
{
    public:
        static void general_error_message(const Filepath& location, std::string message);
        static void general_warning_message(const Filepath& location, std::string message);
        static void parser_error_message(Token error_location, std::string message);
        static void parser_warning_message(Token error_location, std::string message);

        static bool dump(Filepath path_to_dump);

    protected:
        static ErrorHandler& self();

    protected:
        std::vector<ErrorMessageFormat> error_messages;

                        ErrorHandler();
        virtual        ~ErrorHandler();
};

#endif
