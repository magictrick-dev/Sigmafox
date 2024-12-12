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

// --- Syntax Error Exception --------------------------------------------------
//
// 
//

SyntaxError::
SyntaxError()
{
    this->message = "Undefined syntax error exception.";
}

SyntaxError::
~SyntaxError()
{

}

const char* SyntaxError::
what() const
{

    return this->message.c_str();

}
