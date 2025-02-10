#include <compiler/tokenizer/token.hpp>

static inline const std::unordered_map<TokenType, std::string>&
get_token_map()
{

    // The keyword map only needs to be initialized once.
    static std::unordered_map<TokenType, std::string> map;
    static bool initialized_map = false;

    if (initialized_map == false)
    {

        map[TokenType::TOKEN_COMMENT_BLOCK]         = "comment block";
        map[TokenType::TOKEN_LEFT_PARENTHESIS]      = "left parenthesis";
        map[TokenType::TOKEN_RIGHT_PARENTHESIS]     = "right parenthesis";
        map[TokenType::TOKEN_COMMA]                 = "comma";
        map[TokenType::TOKEN_SEMICOLON]             = "semicolon";
        map[TokenType::TOKEN_COLON_EQUALS]          = "colon equals";
        map[TokenType::TOKEN_PLUS]                  = "plus";
        map[TokenType::TOKEN_MINUS]                 = "minus";
        map[TokenType::TOKEN_STAR]                  = "star";
        map[TokenType::TOKEN_FORWARD_SLASH]         = "forward_slash";
        map[TokenType::TOKEN_CARROT]                = "carrot";
        map[TokenType::TOKEN_EQUALS]                = "equals";
        map[TokenType::TOKEN_LESS_THAN]             = "less_than";
        map[TokenType::TOKEN_LESS_THAN_EQUALS]      = "less_than_equals";
        map[TokenType::TOKEN_GREATER_THAN]          = "greater_than";
        map[TokenType::TOKEN_GREATER_THAN_EQUALS]   = "greater_than_equals";
        map[TokenType::TOKEN_HASH]                  = "hash";
        map[TokenType::TOKEN_AMPERSAND]             = "ampersand";
        map[TokenType::TOKEN_PIPE]                  = "pipe";
        map[TokenType::TOKEN_PERCENT]               = "percent";

        map[TokenType::TOKEN_INTEGER]               = "integer";
        map[TokenType::TOKEN_REAL]                  = "real";
        map[TokenType::TOKEN_STRING]                = "string";
        map[TokenType::TOKEN_IDENTIFIER]            = "identifier";

        map[TokenType::TOKEN_KEYWORD_BEGIN]         = "keyword begin";
        map[TokenType::TOKEN_KEYWORD_ELSEIF]        = "keyword elseif";
        map[TokenType::TOKEN_KEYWORD_END]           = "keyword end";
        map[TokenType::TOKEN_KEYWORD_ENDFIT]        = "keyword endfit";
        map[TokenType::TOKEN_KEYWORD_ENDIF]         = "keyword endif";
        map[TokenType::TOKEN_KEYWORD_ENDFUNCTION]   = "keyword endfunction";
        map[TokenType::TOKEN_KEYWORD_ENDLOOP]       = "keyword endloop";
        map[TokenType::TOKEN_KEYWORD_ENDPLOOP]      = "keyword endploop";
        map[TokenType::TOKEN_KEYWORD_ENDPROCEDURE]  = "keyword endprocedure";
        map[TokenType::TOKEN_KEYWORD_ENDSCOPE]      = "keyword endscope";
        map[TokenType::TOKEN_KEYWORD_ENDWHILE]      = "keyword endwhile";
        map[TokenType::TOKEN_KEYWORD_FIT]           = "keyword fit";
        map[TokenType::TOKEN_KEYWORD_FUNCTION]      = "keyword function";
        map[TokenType::TOKEN_KEYWORD_IF]            = "keyword if";
        map[TokenType::TOKEN_KEYWORD_INCLUDE]       = "keyword include";
        map[TokenType::TOKEN_KEYWORD_LOOP]          = "keyword loop";
        map[TokenType::TOKEN_KEYWORD_PLOOP]         = "keyword ploop";
        map[TokenType::TOKEN_KEYWORD_PROCEDURE]     = "keyword procedure";
        map[TokenType::TOKEN_KEYWORD_READ]          = "keyword read";
        map[TokenType::TOKEN_KEYWORD_SAVE]          = "keyword save";
        map[TokenType::TOKEN_KEYWORD_SCOPE]         = "keyword scope";
        map[TokenType::TOKEN_KEYWORD_VARIABLE]      = "keyword variable";
        map[TokenType::TOKEN_KEYWORD_WHILE]         = "keyword while";
        map[TokenType::TOKEN_KEYWORD_WRITE]         = "keyword write";
        map[TokenType::TOKEN_NEW_LINE]              = "new line";

        map[TokenType::TOKEN_EOF]                   = "end-of-file";
        map[TokenType::TOKEN_UNDEFINED]             = "undefined";
        map[TokenType::TOKEN_UNDEFINED_EOF]         = "undefined end-of-file";
        map[TokenType::TOKEN_UNDEFINED_EOL]         = "undefined end-of-line";

    }

    return map;

}

std::string Token::
type_to_string(TokenType type)
{

    auto map = get_token_map();
    return map[type];

}
