#ifndef SIGMAFOX_COMPILER_SCANNER_H
#define SIGMAFOX_COMPILER_SCANNER_H
#include <vector>
#include <string>
#include <compiler/token.h>

class Scanner
{
    public:
                            Scanner(const char *source);

        bool                scan();
    
        std::vector<Token>  tokens;
        std::vector<Token>  invalid_tokens;
        std::string         source;

        bool                error_was_eof_or_eol();
        size_t              last_line();

    private:
        size_t              step;
        size_t              start;
        size_t              line;
        bool                hit_eof;

        char                advance();
        char                peek();

        bool                is_alpha();
        bool                is_numeric();
        bool                is_alphanumeric();
        bool                is_eof();
        bool                is_linecontrol();

        void                add_token(TokenType type);
        void                add_error();

};

#endif
