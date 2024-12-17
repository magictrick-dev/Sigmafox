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
#include <exception>
#include <definitions.hpp>
#include <platform/system.hpp>
#include <compiler/tokenizer.hpp>

// --- Syntax Exceptions -------------------------------------------------------

class SyntaxException : public std::exception
{

    public:
        ccptr what() const = 0;

    public:
        bool handled = false;

};

// --- Syntax Warnings ---------------------------------------------------------
//
// The syntax warnings operate in the same capacity as errors.
//

class SyntaxWarning : public SyntaxException 
{
    public:
        inline          SyntaxWarning();
        inline virtual ~SyntaxWarning();
        template <class... Args> inline SyntaxWarning(const Filepath& location,
                const Token& reference, std::string format, Args... args);

        inline ccptr    what() const override;

    protected:
        std::string message;

};

SyntaxWarning::
SyntaxWarning()
{

}

SyntaxWarning::
~SyntaxWarning()
{

}

template <class... Args> SyntaxWarning::
SyntaxWarning(const Filepath& location, const Token& reference,
        std::string format, Args... args)
{

    // Probably a cleaner way to do this.
    this->message += location.c_str();
    this->message += "(";
    this->message += std::to_string(reference.row);
    this->message += ",";
    this->message += std::to_string(reference.column);
    this->message += ")(warning): ";

    // A little bit easier to work with than snprintf and malloc, I guess.
    i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::string formatted_message(size_s, ' ');
    std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
    formatted_message.pop_back();
    this->message += formatted_message;

}

ccptr SyntaxWarning::
what() const
{

    return this->message.c_str();

}

// --- Syntax Errors -----------------------------------------------------------
//
// Used for error handling in the parser and recovering from syntax errors. Most
// errors are recoverable, but may propagate higher, thus the need for the handled
// flag in the structure.
//

class SyntaxError : public SyntaxException 
{
    public:
        inline          SyntaxError();
        inline virtual ~SyntaxError();
        template <class... Args> inline SyntaxError(const Filepath& location,
                const Token& reference, std::string format, Args... args);

        inline ccptr    what() const override;

    protected:
        std::string message;

};

SyntaxError::
SyntaxError()
{

}

SyntaxError::
~SyntaxError()
{

}


template <class... Args> SyntaxError::
SyntaxError(const Filepath& location, const Token& reference,
        std::string format, Args... args)
{

    // Probably a cleaner way to do this.
    this->message += location.c_str();
    this->message += "(";
    this->message += std::to_string(reference.row);
    this->message += ",";
    this->message += std::to_string(reference.column);
    this->message += ")(error): ";

    // A little bit easier to work with than snprintf and malloc, I guess.
    i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::string formatted_message(size_s, ' ');
    std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
    formatted_message.pop_back();
    this->message += formatted_message;

}

ccptr SyntaxError::
what() const
{

    return this->message.c_str();

}

#endif
