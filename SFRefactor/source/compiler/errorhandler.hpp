// --- Sigmafox Compiler Error Handler -----------------------------------------
//
// Written by Chris DeJong Nov. 2024 / Northern Illinois University
//
//      The error handler routine is designed to offer a cohesive interface
//      method for displaying human-readable errors to the user. Error messages
//      should provide a where and how, as well as a potential suggestion for
//      fixing the issue.
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_COMPILER_ERROR_HANDLER_HPP
#define SIGMAFOX_COMPILER_ERROR_HANDLER_HPP
#include <vector>
#include <string>
#include <definitions.hpp>
#include <platform/system.hpp>
#include <compiler/tokenizer.hpp>

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
        template <class... Args> static void parse_error(const Filepath& location, const Token& reference, 
                std::string format, Args... args);

    protected:
        static ErrorHandler& self();

    protected:
        std::vector<ErrorMessageFormat> error_messages;

                        ErrorHandler();
        virtual        ~ErrorHandler();
};

template <class... Args> void ErrorHandler::
parse_error(const Filepath& location, const Token& reference, 
        std::string format, Args... args)
{

    // Probably a cleaner way to do this.
    std::string output_message;
    output_message += location.c_str();
    output_message += "(";
    output_message += std::to_string(reference.row);
    output_message += ",";
    output_message += std::to_string(reference.column);
    output_message += ")(error): ";

    // A little bit easier to work with than snprintf and malloc, I guess.
    i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::string formatted_message(size_s, ' ');
    std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
    formatted_message.pop_back();
    output_message += formatted_message;

    ErrorHandler::self().error_messages.push_back({
        .filepath = location.c_str(),
        .message = formatted_message,
        .offender = reference.reference,
        .column_location = reference.column,
        .line_location = reference.row,
        .timestamp = 0,
    });

    std::cout << output_message << std::endl;

}

#endif
