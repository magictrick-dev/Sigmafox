#include <cstdarg>
#include <unordered_map>
#include <environment.hpp>
#include <compiler/tokenizer.hpp>
#include <platform/filesystem.hpp>

// NOTE(Chris): The keyword map is case-insensitive, we enforce upper-case here.
//              We are statically lazy-initializing this since we only need to
//              reference this map later on. Marked as const because that's what
//              John Carmack would have wanted.
static inline const std::unordered_map<std::string, TokenType>& 
get_keyword_map()
{

    // The keyword map only needs to be initialized once.
    static std::unordered_map<std::string, TokenType> map;
    static bool initialized_map = false;

    if (initialized_map == false)
    {
        
        map["BEGIN"]        = TokenType::TOKEN_KEYWORD_BEGIN;
        map["ELSEIF"]       = TokenType::TOKEN_KEYWORD_ELSEIF;
        map["END"]          = TokenType::TOKEN_KEYWORD_END;
        map["ENDFIT"]       = TokenType::TOKEN_KEYWORD_ENDFIT;
        map["ENDIF"]        = TokenType::TOKEN_KEYWORD_ENDIF;
        map["ENDFUNCTION"]  = TokenType::TOKEN_KEYWORD_ENDFUNCTION;
        map["ENDLOOP"]      = TokenType::TOKEN_KEYWORD_ENDLOOP;
        map["ENDPLOOP"]     = TokenType::TOKEN_KEYWORD_ENDPLOOP;
        map["ENDPROCEDURE"] = TokenType::TOKEN_KEYWORD_ENDPROCEDURE;
        map["ENDSCOPE"]     = TokenType::TOKEN_KEYWORD_ENDSCOPE;
        map["ENDWHILE"]     = TokenType::TOKEN_KEYWORD_ENDWHILE;
        map["FIT"]          = TokenType::TOKEN_KEYWORD_FIT;
        map["FUNCTION"]     = TokenType::TOKEN_KEYWORD_FUNCTION;
        map["IF"]           = TokenType::TOKEN_KEYWORD_IF;
        map["INCLUDE"]      = TokenType::TOKEN_KEYWORD_INCLUDE;
        map["LOOP"]         = TokenType::TOKEN_KEYWORD_LOOP;
        map["PLOOP"]        = TokenType::TOKEN_KEYWORD_PLOOP;
        map["PROCEDURE"]    = TokenType::TOKEN_KEYWORD_PROCEDURE;
        map["READ"]         = TokenType::TOKEN_KEYWORD_READ;
        map["SAVE"]         = TokenType::TOKEN_KEYWORD_SAVE;
        map["SCOPE"]        = TokenType::TOKEN_KEYWORD_SCOPE;
        map["VARIABLE"]     = TokenType::TOKEN_KEYWORD_VARIABLE;
        map["WHILE"]        = TokenType::TOKEN_KEYWORD_WHILE;
        map["WRITE"]        = TokenType::TOKEN_KEYWORD_WRITE;

        initialized_map = true;

    }

    return map;

}

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

// --- Tokenizer Implementation ------------------------------------------------
//
// The implementation of the tokenizer is pretty straight forward, but it does
// rely on some helper routines to make this work.
//

Tokenizer::
Tokenizer(const Filepath& path)
{

    SF_ASSERT(path.is_valid_file());

    // Set the path.
    this->path = path;

    // Read the file into the tokenizer.
    u64 size = file_size(path.c_str());
    this->source.resize(size+1);
    u64 read_size = file_read_all(path.c_str(), this->source.data(), size);
    SF_ASSERT(size == read_size);

    // Set our token buffers.
    this->previous_token    = &token_buffer[0];
    this->current_token     = &token_buffer[1];
    this->next_token        = &token_buffer[2];

    // Valid initialize our tokens to EOF such that they're known values.
    for (i32 i = 0; i < 3; ++i)
    {
        this->token_buffer[i].type      = TokenType::TOKEN_EOF;
        this->token_buffer[i].reference = "";
        this->token_buffer[i].row       = 0;
        this->token_buffer[i].column    = 0;
    }

    // This will prime the current and next tokens automatically for us.
    this->shift(); // Current token.
    this->shift(); // Next token.

}

Tokenizer::
~Tokenizer()
{

}

void Tokenizer::
set_token(Token *token, TokenType type)
{

    token->reference.clear();
    ccptr offset_string = this->source.c_str() + this->offset;
    i32 length = this->step - this->offset;
    for (i32 i = 0; i < length; ++i) token->reference += offset_string[i];

    token->type     = type;
    token->row      = row;
    token->column   = column - length;

}

char Tokenizer::
consume(u32 count)
{

    char current = this->source[this->step];
    for (u32 i = 0; i < count; ++i)
    {
        current = this->source[this->step];
        if (current == '\n')
        {
            this->row += 1;
            this->column = 0;
        }
        this->step += 1;
        this->column += 1;
    }

    return current;

}

char Tokenizer::
peek(u32 offset)
{

    char peek = this->source[this->step + offset];
    return peek;

}

void Tokenizer::
synchronize()
{

    this->offset = this->step;
    return;

}

b32 Tokenizer::
is_eof() const
{

    if (this->source[this->step] == '\0')
        return true;
    return false;

}

b32 Tokenizer::
is_eol() const
{
    
    if (this->source[this->step] == '\n')
        return true;
    return false;

}

TokenType Tokenizer::
check_identifier() const
{

    const std::unordered_map<std::string, TokenType>& keyword_map = get_keyword_map();
    std::string identifier = this->next_token->reference;
    for (auto &c : identifier) c = toupper(c); // Simpler this way, keywords are case insensitive.

    TokenType result = TokenType::TOKEN_IDENTIFIER;
    auto find_iter = keyword_map.find(identifier); // The only true useful thing about auto.
    if (find_iter != keyword_map.end()) result = find_iter->second;
    return result;

}

b32 Tokenizer::
consume_whitespace()
{

    // NOTE(Chris): There might be additional whitespace characters to consider
    //              here that we might not have caught. On these edge cases, we
    //              should probably check for them.
    if (this->match_characters(4, '\t', '\r', '\n', ' '))
    {
        this->consume(1);
        this->synchronize();
        return true;
    }

    else if (this->match_characters(1, '{'))
    {

        // Consumes everything after the '{'.
        while (this->peek(0) != '}' && !this->is_eof())
        {
            this->consume(1);
        }

        // The comment could reach EOF, so account for that case and
        // generate the appropriate error token.
        if (this->is_eof())
        {

            this->set_token(this->next_token, TokenType::TOKEN_UNDEFINED_EOF);

        }

        this->consume(1);
        this->synchronize();
        return true;

    }

    return false;

}

b32 Tokenizer::
match_characters(u32 count, ...)
{

    // TODO(Chris): We don't need to use va_list for this, we can use an initializer list
    //              or some other C++ construct to solve this problem. For now, leave it
    //              as is since it is working as intended.
    va_list args;
    va_start(args, count);
    b32 matched = false;

    char current = this->peek(0);
    for (u32 idx = 0; idx < count; ++idx)
    {
        char argument_character = va_arg(args, char);
        if (argument_character == current)
        {
            matched = true;
            break;
        }
    }

    va_end(args);

    return matched;

}

b32 Tokenizer::
match_newline()
{

    char current = this->peek(0);
    if (current == '\n')
    {
        this->consume(1);
        this->set_token(this->next_token, TokenType::TOKEN_NEW_LINE);
        this->synchronize();
        return true;
    }

    return false;

}

b32 Tokenizer::
match_comments()
{

    char current = this->peek(0);
    if (current == '{')
    {

        // Consumes everything after the '{'.
        while (this->peek(0) != '}' && !this->is_eof()) this->consume(1);

        // The comment could reach EOF, so account for that case and
        // generate the appropriate error token.
        if (this->is_eof())
        {

            this->set_token(this->next_token, TokenType::TOKEN_UNDEFINED_EOF);

        }
        else
        {

            this->consume(1); // Consume trailing '}', this isn't in the next token.
            this->set_token(this->next_token, TokenType::TOKEN_COMMENT_BLOCK);

        }

        this->synchronize();
        return true;

    }

    return false;
}

b32 Tokenizer::
match_symbols()
{

    char peek = this->peek(0);
    switch (peek)
    {
        case '(':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_LEFT_PARENTHESIS);
            this->synchronize();
            return true;
        } break;

        case ')':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_RIGHT_PARENTHESIS);
            this->synchronize();
            return true;
        } break;

        case ',':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_COMMA);
            this->synchronize();
            return true;
        } break;
        
        case ';':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_SEMICOLON);
            this->synchronize();
            return true;
        } break;

        case '+':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_PLUS);
            this->synchronize();
            return true;
        } break;

        case '-':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_MINUS);
            this->synchronize();
            return true;
        } break;
        
        case '*':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_STAR);
            this->synchronize();
            return true;
        } break;

        case '/':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_FORWARD_SLASH);
            this->synchronize();
            return true;
        } break;

        case '^':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_CARROT);
            this->synchronize();
            return true;
        } break;

        case '=':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_EQUALS);
            this->synchronize();
            return true;
        } break;

        case '#':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_HASH);
            this->synchronize();
            return true;
        } break;
        
        case '&':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_AMPERSAND);
            this->synchronize();
            return true;
        } break;

        case '|':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_PIPE);
            this->synchronize();
            return true;
        } break;

        case '%':
        {
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_PERCENT);
            this->synchronize();
            return true;
        } break;

        case '<':
        {
            char follower = this->peek(1);

            if (follower == '=')
            {
                this->consume(2);
                this->set_token(this->next_token, TokenType::TOKEN_LESS_THAN_EQUALS);
                this->synchronize();
                return true;
            }
            
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_LESS_THAN);
            this->synchronize();
            return true;

        } break;

        case '>':
        {
            char follower = this->peek(1);

            if (follower == '=')
            {
                this->consume(2);
                this->set_token(this->next_token, TokenType::TOKEN_GREATER_THAN_EQUALS);
                this->synchronize();
                return true;
            }
            
            this->consume(1);
            this->set_token(this->next_token, TokenType::TOKEN_GREATER_THAN);
            this->synchronize();
            return true;

        } break;

        case ':':
        {
            
            char follower = this->peek(1);
            if (follower == '=')
            {
                this->consume(2);
                this->set_token(this->next_token, TokenType::TOKEN_COLON_EQUALS);
                this->synchronize();
                return true;
            }

            return false;

        } break;


    };

    return false;

}

b32 Tokenizer::
match_numbers()
{

    char head = this->peek(0);
    if (isdigit(head))
    {

        this->consume(1);
        TokenType type = TokenType::TOKEN_INTEGER;
        
        while (true)
        {

            // Handle decimals.
            char peek = this->peek(0);
            if (peek == '.')
            {
                char forward = this->peek(1);
                if (isdigit(forward))
                {

                    this->consume(2);
                    type = TokenType::TOKEN_REAL;
                    continue;

                }
                else
                {
                    break;
                }
            }

            // Consume until non-number.
            if (isdigit(peek))
            {

                this->consume(1);
                continue;

            }
            else
            {
                break;
            }

        }

        this->set_token(this->next_token, type);
        this->synchronize();
        return true;

    }

    return false;

}

b32 Tokenizer::
match_strings()
{

    char current = this->peek(0);
    char left_side = current;
    if (current == '\'' || current == '\"')
    {

        this->consume(1);
        this->synchronize();

        while (this->peek(0) != left_side && !this->is_eof() && !this->is_eol())
            this->consume(1);

        // Strings potentially terminate at EOF or EOL, so we check both cases.
        if (this->is_eof())
        {
            this->set_token(this->next_token, TokenType::TOKEN_UNDEFINED_EOF);
        }

        else if (this->is_eol())
        {
            this->set_token(this->next_token, TokenType::TOKEN_UNDEFINED_EOL);
        }

        else
        {

            this->set_token(this->next_token, TokenType::TOKEN_STRING);
            this->consume(1);

        }

        this->synchronize();
        return true;

    }

    return false;

}

b32 Tokenizer::
match_identifiers()
{

    char head = this->peek(0);
    if (isalpha(head))
    {

        // Consume until keyword match breaks.
        this->consume(1);
        while (true)
        {
            char peek = this->peek(0);
            if (peek == '_' || isalnum(peek))
            {
                this->consume(1);
                continue;
            }
            else
            {
                break;
            }
        }

        this->set_token(this->next_token, TokenType::TOKEN_IDENTIFIER);

        // Convert identifiers to keywords if they're keywords.
        this->next_token->type = this->check_identifier();
        this->synchronize();
        return true;

    }

    return false;

}

void Tokenizer::
shift()
{

    // Shift our tokens left.
    Token *temporary            = this->previous_token;
    this->previous_token        = this->current_token;
    this->current_token         = this->next_token;
    this->next_token            = temporary;
    
    // Clear whitespace.
    while (this->consume_whitespace());

    // Check if we're at the end of file and if we are, set the token to EOF.
    if (this->is_eof())
    {
        this->set_token(this->next_token, TokenType::TOKEN_EOF);
        return;
    }

    // Match to specification.
    if (this->match_symbols())      return;
    if (this->match_numbers())      return;
    if (this->match_strings())      return;
    if (this->match_identifiers())  return;
    
    // If we're here, we didn't match to specification, the token is undefined.
    this->consume(1);
    this->set_token(this->next_token, TokenType::TOKEN_UNDEFINED);
    this->synchronize(); // Synchronize.
    return;

}

Token Tokenizer::
get_previous_token() const
{
    
    return *this->previous_token;

}

Token Tokenizer::
get_current_token() const
{
    
    return *this->current_token;

}

Token Tokenizer::
get_next_token() const
{
    
    return *this->next_token;

}

b32 Tokenizer::
previous_token_is(TokenType type) const
{

    b32 result = (this->previous_token->type == type);
    return result;

}

b32 Tokenizer::
current_token_is(TokenType type) const
{

    b32 result = (this->current_token->type == type);
    return result;

}

b32 Tokenizer::
next_token_is(TokenType type) const
{

    b32 result = (this->next_token->type == type);
    return result;

}
