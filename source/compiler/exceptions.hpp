#ifndef SIGAMFOX_COMPILER_EXCEPTIONS_HPP
#define SIGAMFOX_COMPILER_EXCEPTIONS_HPP
#include <definitions.hpp>
#include <exception>

// --- Compiler Exception ------------------------------------------------------
// 
// A base class implementation which serves as a catch all for exceptions thrown
// by the compiler.
//

class CompilerException : public std::exception
{

    public:
        ccptr what() const noexcept = 0;


};

// --- Compiler Syntax Error ---------------------------------------------------
// 
// A class implementation which serves as a catch all for syntax errors thrown.
// 

class CompilerSyntaxError : public CompilerException
{

    public:
        template <typename ...Args> 
        CompilerSyntaxError(u64 line_location, u64 line, u64 column, std::string path,
            std::string format, Args... args)
        {
            
            message += "[" + std::to_string(line_location) 
                + "]: " + path + "(" + std::to_string(line) + ", " + std::to_string(column) + ")(Error): ";
            
            // TODO(Chris): Clearly, there is a better way to do this, right? Primary candidate
            //              for a custom intrinsic.
#           if defined(__linux__)
#           pragma GCC diagnostic push
#           pragma GCC diagnostic ignored "-Wformat-security"
            i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
            string formatted_message(size_s, ' ');
            std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
            formatted_message.pop_back();
#           pragma GCC diagnostic pop
#           else
            i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
            string formatted_message(size_s, ' ');
            std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
            formatted_message.pop_back();
#           endif
            
            message += formatted_message;

        }
        
        ccptr what() const noexcept override
        {
            return message.c_str();
        }
        
    protected:
        string message;

};

class CompilerSyntaxWarning : public CompilerException
{

    public:
        template <typename ...Args> 
        CompilerSyntaxWarning(u64 line_location, u64 line, u64 column, std::string path,
            std::string format, Args... args)
        {
            
            message += "[" + std::to_string(line_location) 
                + "]: " + path + "(" + std::to_string(line) + ", " + std::to_string(column) + ")(Warning): ";
            
            // TODO(Chris): Clearly, there is a better way to do this, right? Primary candidate
            //              for a custom intrinsic.
#           if defined(__linux__)
#           pragma GCC diagnostic push
#           pragma GCC diagnostic ignored "-Wformat-security"
            i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
            string formatted_message(size_s, ' ');
            std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
            formatted_message.pop_back();
#           pragma GCC diagnostic pop
#           else
            i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
            string formatted_message(size_s, ' ');
            std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
            formatted_message.pop_back();
#           endif
            
            message += formatted_message;

        }

        ccptr what() const noexcept override
        {
            return message.c_str();
        }
        
    protected:
        string message;

};

class CompilerSyntaxInformation : public CompilerException
{

    public:
        template <typename ...Args> 
        CompilerSyntaxInformation(u64 line_location, u64 line, u64 column, 
            std::string path, std::string format, Args... args)
        {
            
            message += "[" + std::to_string(line_location) 
                + "]: " + path + "(" + std::to_string(line) + ", " + std::to_string(column) + ")(Error): ";
            
            // TODO(Chris): Clearly, there is a better way to do this, right? Primary candidate
            //              for a custom intrinsic.
#           if defined(__linux__)
#           pragma GCC diagnostic push
#           pragma GCC diagnostic ignored "-Wformat-security"
            i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
            string formatted_message(size_s, ' ');
            std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
            formatted_message.pop_back();
#           pragma GCC diagnostic pop
#           else
            i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
            string formatted_message(size_s, ' ');
            std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
            formatted_message.pop_back();
#           endif
            
            message += formatted_message;

        }

        ccptr what() const noexcept override
        {
            return message.c_str();
        }
        
    protected:
        string message;

};

class CompilerEvaluatorError : public CompilerException
{

    public:
        template <typename ...Args> 
        CompilerEvaluatorError(u64 line_location, std::string format, Args... args)
        {

            message += "[" + std::to_string(line_location) + "]: ";
           
            // TODO(Chris): Clearly, there is a better way to do this, right? Primary candidate
            //              for a custom intrinsic.
#           if defined(__linux__)
#           pragma GCC diagnostic push
#           pragma GCC diagnostic ignored "-Wformat-security"
            i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
            string formatted_message(size_s, ' ');
            std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
            formatted_message.pop_back();
#           pragma GCC diagnostic pop
#           else
            i32 size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
            string formatted_message(size_s, ' ');
            std::snprintf(formatted_message.data(), size_s, format.c_str(), args...);
            formatted_message.pop_back();
#           endif
            
            message += formatted_message;

        }
        
        ccptr what() const noexcept override
        {
            return message.c_str();
        }
        
    protected:
        string message;

};

#endif
