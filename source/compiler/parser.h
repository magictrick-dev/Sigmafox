#ifndef SIGMAFOX_COMPILER_PARSER_H
#define SIGMAFOX_COMPILER_PARSER_H
#include <vector>
#include <allocator.h>
#include <compiler/grammar.h>

class SyntaxErrorException
{

    public:
        inline SyntaxErrorException(Token error_token, std::string message)
        {
            this->error_token = error_token;
            this->message = message;
        }

        inline std::string to_string()
        {
            std::string error_message = message + " at line(";
            error_message += std::to_string(this->error_token.line) + "): '";
            error_message += this->error_token.lexeme + "'";
            return error_message;
        }

    protected:
        std::string message;
        Token error_token;

};

class Parser
{
    public:
        inline  Parser(std::vector<Token> tokens) : tokens(tokens) {};

        std::vector<sharedptr<Statement>> parse();
        bool                    is_eof();

        sharedptr<Expression>   expression();

    protected:
        sharedptr<Expression>   equality();
        sharedptr<Expression>   comparison();
        sharedptr<Expression>   term();
        sharedptr<Expression>   factor();
        sharedptr<Expression>   unary();
        sharedptr<Expression>   primary();

        sharedptr<Statement>    statement();
        sharedptr<Statement>    print_statement();
        sharedptr<Statement>    expression_statement();

        void                    synchronize();

        bool                    match(std::vector<TokenType> types);
        bool                    check(TokenType type);
        Token                   consume(TokenType type, std::string err_message);
        Token                   advance();
        Token                   previous();
        Token                   peek();

    protected:
        std::vector<Token>  tokens;
        size_t              current = 0;


};

#endif
