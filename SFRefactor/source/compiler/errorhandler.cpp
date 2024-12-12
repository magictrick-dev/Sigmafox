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
