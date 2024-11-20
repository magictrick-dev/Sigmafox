#include <compiler/errorhandler.hpp>

ErrorHandler::
ErrorHandler()
{

}

ErrorHandler::
~ErrorHandler()
{

}

ErrorHandler& ErrorHandler::
self()
{

    static ErrorHandler instance;
    return instance;

}

void ErrorHandler::
general_error_message(const Filepath& location, std::string message)
{

    return;
}

void ErrorHandler::
general_warning_message(const Filepath& location, std::string message)
{

    return;
}

void ErrorHandler::
parser_error_message(Token error_location, std::string message)
{

    return;
}

void ErrorHandler::
parser_warning_message(Token error_location, std::string message)
{

    return;
}
