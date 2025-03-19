#include <compiler/tokenizer/token.hpp>

static inline const std::unordered_map<Tokentype, std::string>&
get_token_map()
{

    // The keyword map only needs to be initialized once.
    static std::unordered_map<Tokentype, std::string> map;
    static bool initialized_map = false;

    if (initialized_map == false)
    {

        map[Tokentype::TOKEN_COMMENT_BLOCK]         = "comment block";
        map[Tokentype::TOKEN_LEFT_PARENTHESIS]      = "left parenthesis";
        map[Tokentype::TOKEN_RIGHT_PARENTHESIS]     = "right parenthesis";
        map[Tokentype::TOKEN_COMMA]                 = "comma";
        map[Tokentype::TOKEN_SEMICOLON]             = "semicolon";
        map[Tokentype::TOKEN_COLON_EQUALS]          = "colon equals";
        map[Tokentype::TOKEN_PLUS]                  = "plus";
        map[Tokentype::TOKEN_MINUS]                 = "minus";
        map[Tokentype::TOKEN_STAR]                  = "star";
        map[Tokentype::TOKEN_FORWARD_SLASH]         = "forward_slash";
        map[Tokentype::TOKEN_CARROT]                = "carrot";
        map[Tokentype::TOKEN_EQUALS]                = "equals";
        map[Tokentype::TOKEN_LESS_THAN]             = "less_than";
        map[Tokentype::TOKEN_LESS_THAN_EQUALS]      = "less_than_equals";
        map[Tokentype::TOKEN_GREATER_THAN]          = "greater_than";
        map[Tokentype::TOKEN_GREATER_THAN_EQUALS]   = "greater_than_equals";
        map[Tokentype::TOKEN_HASH]                  = "hash";
        map[Tokentype::TOKEN_AMPERSAND]             = "ampersand";
        map[Tokentype::TOKEN_PIPE]                  = "pipe";
        map[Tokentype::TOKEN_PERCENT]               = "percent";

        map[Tokentype::TOKEN_INTEGER]               = "integer";
        map[Tokentype::TOKEN_REAL]                  = "real";
        map[Tokentype::TOKEN_STRING]                = "string";
        map[Tokentype::TOKEN_IDENTIFIER]            = "identifier";

        map[Tokentype::TOKEN_KEYWORD_BEGIN]         = "keyword begin";
        map[Tokentype::TOKEN_KEYWORD_ELSEIF]        = "keyword elseif";
        map[Tokentype::TOKEN_KEYWORD_END]           = "keyword end";
        map[Tokentype::TOKEN_KEYWORD_ENDFIT]        = "keyword endfit";
        map[Tokentype::TOKEN_KEYWORD_ENDIF]         = "keyword endif";
        map[Tokentype::TOKEN_KEYWORD_ENDFUNCTION]   = "keyword endfunction";
        map[Tokentype::TOKEN_KEYWORD_ENDLOOP]       = "keyword endloop";
        map[Tokentype::TOKEN_KEYWORD_ENDPLOOP]      = "keyword endploop";
        map[Tokentype::TOKEN_KEYWORD_ENDPROCEDURE]  = "keyword endprocedure";
        map[Tokentype::TOKEN_KEYWORD_ENDSCOPE]      = "keyword endscope";
        map[Tokentype::TOKEN_KEYWORD_ENDWHILE]      = "keyword endwhile";
        map[Tokentype::TOKEN_KEYWORD_FIT]           = "keyword fit";
        map[Tokentype::TOKEN_KEYWORD_FUNCTION]      = "keyword function";
        map[Tokentype::TOKEN_KEYWORD_IF]            = "keyword if";
        map[Tokentype::TOKEN_KEYWORD_INCLUDE]       = "keyword include";
        map[Tokentype::TOKEN_KEYWORD_LOOP]          = "keyword loop";
        map[Tokentype::TOKEN_KEYWORD_PLOOP]         = "keyword ploop";
        map[Tokentype::TOKEN_KEYWORD_PROCEDURE]     = "keyword procedure";
        map[Tokentype::TOKEN_KEYWORD_READ]          = "keyword read";
        map[Tokentype::TOKEN_KEYWORD_SAVE]          = "keyword save";
        map[Tokentype::TOKEN_KEYWORD_SCOPE]         = "keyword scope";
        map[Tokentype::TOKEN_KEYWORD_VARIABLE]      = "keyword variable";
        map[Tokentype::TOKEN_KEYWORD_WHILE]         = "keyword while";
        map[Tokentype::TOKEN_KEYWORD_WRITE]         = "keyword write";
        map[Tokentype::TOKEN_NEW_LINE]              = "new line";

        map[Tokentype::TOKEN_EOF]                   = "end-of-file";
        map[Tokentype::TOKEN_UNDEFINED]             = "undefined";
        map[Tokentype::TOKEN_UNDEFINED_EOF]         = "undefined end-of-file";
        map[Tokentype::TOKEN_UNDEFINED_EOL]         = "undefined end-of-line";

    }

    return map;

}

std::string Token::
type_to_string(Tokentype type)
{

    auto map = get_token_map();
    return map[type];

}
