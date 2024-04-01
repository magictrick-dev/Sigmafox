#ifndef SIGMAFOX_LEXER_H
#define SIGMAFOX_LEXER_H
#include <core.h>
#include <vector>
#include <string>

enum class TokenType
{

    // Symbols
    LEFT_CURLY_BRACKET,     // {
    RIGHT_CURLY_BRACKET,    // }
    SEMICOLON,              // ;
    ASSIGNMENT_OPERATOR,    // :=
    PLUS,                   // +
    MINUS,                  // -
    MULTIPLY,               // *
    DIVISION,               // /
    POWER,                  // **
    LESSTHAN,               // <
    GREATERTHAN,            // >
    EQUALS,                 // =
    NOTEQUALS,              // #
    CONCAT,                 // &
    EXTRACT,                // |
    DERIVATION,             // %

    // Definables
    IDENTIFIER,
    STRING_SINGLE,
    STRING_DOUBLE,
    NUMBER,

    // Keywords
    BEGIN,      END,
    PROCEDURE,  ENDPROCEDURE,
    FUNCTION,   ENDFUNCTION,
    IF,         ENDIF,
    WHILE,      ENDWHILE,
    LOOP,       ENDLOOP,
    PLOOP,      ENDPLOOP,
    FIT,        ENDFIT,
    VARIABLE,
    WRITE,
    READ,
    SAVE,
    INCLUDE,

    // Unidentified
    UNDEFINED,

};

struct Lexeme
{
    const char *reference;
    size_t      offset;
    size_t      length;
    size_t      line_number;
    size_t      column_number;
};

class Token
{
    public:
                    Token(Lexeme lexeme, TokenType type);

        std::string to_string() const;
        size_t      get_offset() const;
        size_t      get_length() const;
        size_t      get_line() const;
        size_t      get_column() const;

    protected:
        Lexeme      lexeme;
        TokenType   type;

};

class Lex
{
    
    public:
                        Lex(const char *source, const char *source_path);

        bool            has_errors() const;
        void            print_errors() const;
        void            print_tokens() const;

    protected:
        void            parse();
        void            push_error(Token token);
        void            push_error(Token token, std::string message);

        bool            is_eof() const;
        char            advance();
        char            peek();

        bool            is_number(char c);
        bool            is_alpha(char c);
        bool            is_alphanumeric(char c);
        bool            is_linecontrol(char c);
        bool            is_eof(char c);
        TokenType       check_keyword(Token token);

        void            add_token(size_t offset, size_t length, TokenType type);

    protected:
        char                        path[260];
        const char                 *source;
        std::vector<Token>          tokens;
        std::vector<std::string>    errors;

        size_t          step;
        size_t          line_number;
        size_t          line_offset;
        size_t          source_length;
        bool            has_error;

};

#endif
