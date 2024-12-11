#include <cstdarg>
#include <unordered_map>
#include <environment.hpp>
#include <compiler/tokenizer.hpp>

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

        map[TokenType::TOKEN_COMMENT_BLOCK]         = "token_comment_block";
        map[TokenType::TOKEN_LEFT_PARENTHESIS]      = "token_left_parenthesis";
        map[TokenType::TOKEN_RIGHT_PARENTHESIS]     = "token_right_parenthesis";
        map[TokenType::TOKEN_COMMA]                 = "token_comma";
        map[TokenType::TOKEN_SEMICOLON]             = "token_semicolon";
        map[TokenType::TOKEN_COLON_EQUALS]          = "token_colon_equals";
        map[TokenType::TOKEN_PLUS]                  = "token_plus";
        map[TokenType::TOKEN_MINUS]                 = "token_minus";
        map[TokenType::TOKEN_STAR]                  = "token_star";
        map[TokenType::TOKEN_FORWARD_SLASH]         = "token_forward_slash";
        map[TokenType::TOKEN_CARROT]                = "token_carrot";
        map[TokenType::TOKEN_EQUALS]                = "token_equals";
        map[TokenType::TOKEN_LESS_THAN]             = "token_less_than";
        map[TokenType::TOKEN_LESS_THAN_EQUALS]      = "token_less_than_equals";
        map[TokenType::TOKEN_GREATER_THAN]          = "token_greater_than";
        map[TokenType::TOKEN_GREATER_THAN_EQUALS]   = "token_greater_than_equals";
        map[TokenType::TOKEN_HASH]                  = "token_hash";
        map[TokenType::TOKEN_AMPERSAND]             = "token_ampersand";
        map[TokenType::TOKEN_PIPE]                  = "token_pipe";
        map[TokenType::TOKEN_PERCENT]               = "token_percent";

        map[TokenType::TOKEN_INTEGER]               = "token_integer";
        map[TokenType::TOKEN_REAL]                  = "token_real";
        map[TokenType::TOKEN_STRING]                = "token_string";
        map[TokenType::TOKEN_IDENTIFIER]            = "token_identifier";

        map[TokenType::TOKEN_KEYWORD_BEGIN]         = "token_keyword_begin";
        map[TokenType::TOKEN_KEYWORD_ELSEIF]        = "token_keyword_elseif";
        map[TokenType::TOKEN_KEYWORD_END]           = "token_keyword_end";
        map[TokenType::TOKEN_KEYWORD_ENDFIT]        = "token_keyword_endfit";
        map[TokenType::TOKEN_KEYWORD_ENDIF]         = "token_keyword_endif";
        map[TokenType::TOKEN_KEYWORD_ENDFUNCTION]   = "token_keyword_endfunction";
        map[TokenType::TOKEN_KEYWORD_ENDLOOP]       = "token_keyword_endloop";
        map[TokenType::TOKEN_KEYWORD_ENDPLOOP]      = "token_keyword_endploop";
        map[TokenType::TOKEN_KEYWORD_ENDPROCEDURE]  = "token_keyword_endprocedure";
        map[TokenType::TOKEN_KEYWORD_ENDSCOPE]      = "token_keyword_endscope";
        map[TokenType::TOKEN_KEYWORD_ENDWHILE]      = "token_keyword_endwhile";
        map[TokenType::TOKEN_KEYWORD_FIT]           = "token_keyword_fit";
        map[TokenType::TOKEN_KEYWORD_FUNCTION]      = "token_keyword_function";
        map[TokenType::TOKEN_KEYWORD_IF]            = "token_keyword_if";
        map[TokenType::TOKEN_KEYWORD_INCLUDE]       = "token_keyword_include";
        map[TokenType::TOKEN_KEYWORD_LOOP]          = "token_keyword_loop";
        map[TokenType::TOKEN_KEYWORD_PLOOP]         = "token_keyword_ploop";
        map[TokenType::TOKEN_KEYWORD_PROCEDURE]     = "token_keyword_procedure";
        map[TokenType::TOKEN_KEYWORD_READ]          = "token_keyword_read";
        map[TokenType::TOKEN_KEYWORD_SAVE]          = "token_keyword_save";
        map[TokenType::TOKEN_KEYWORD_SCOPE]         = "token_keyword_scope";
        map[TokenType::TOKEN_KEYWORD_VARIABLE]      = "token_keyword_variable";
        map[TokenType::TOKEN_KEYWORD_WHILE]         = "token_keyword_while";
        map[TokenType::TOKEN_KEYWORD_WRITE]         = "token_keyword_write";
        map[TokenType::TOKEN_NEW_LINE]              = "token_new_line";

        map[TokenType::TOKEN_EOF]                   = "token_eof";
        map[TokenType::TOKEN_UNDEFINED]             = "token_undefined";
        map[TokenType::TOKEN_UNDEFINED_EOF]         = "token_undefined_eof";
        map[TokenType::TOKEN_UNDEFINED_EOL]         = "token_undefined_eol";

    }

    return map;

}

// --- Token Implementation ----------------------------------------------------
// 
// The token class is pretty basic, but we want to have a few helpers to inspect
// them during tokenization and error-handling.
//

Token::
Token()
{

    this->type      = TokenType::TOKEN_UNDEFINED;
    this->resource  = -1;
    this->offset    = 0;
    this->length    = 0;

}

Token::
~Token()
{

}

std::string Token::
to_string() const
{

    // NOTE(Chris): This is a naughty function that does naughty things. We may
    //              want to do something differently to handle this procedure to
    //              make it less explosive.
    ResourceManager& resmanager = ApplicationParameters::get().get_resource_manager();
    SF_ASSERT(resmanager.is_loaded(this->resource));

    cptr modifiable_source = (cptr)resmanager.get_resource(this->resource);

    std::string result;
    cptr offset_string = modifiable_source + this->offset;
    for (i32 i = 0; i < this->length; ++i)
    {
        char c = offset_string[i];
        if (c == '\n') result += "\\n";
        else result += c;
    }

    return result;

}

std::string Token::
type_to_string() const
{

    const std::unordered_map<TokenType, std::string> &map = get_token_map();
    auto iter = map.find(this->type);
    SF_ASSERT(iter != map.end()); // This should never occur since all types must be defined.
    return iter->second;

}

std::pair<i32, i32> Token::
position() const
{

    std::pair<i32, i32> result;

    ResourceManager& resmanager = ApplicationParameters::get().get_resource_manager();
    SF_ASSERT(resmanager.is_loaded(this->resource));
    
    // Count lines & column position.
    ccptr source = resmanager.get_resource_as_text(this->resource);
    int line_count = 1;
    int offset = 0;
    for (i32 i = 0; i < this->offset; ++i)
    {
        offset++;
        if (source[i] == '\n')
        {
            line_count++;
            offset = 0;
        }
    }

    result.first    = line_count;
    result.second   = offset;
    return result;

}

// --- Tokenizer Implementation ------------------------------------------------
//
// The implementation of the tokenizer is pretty straight forward, but it does
// rely on some helper routines to make this work.
//

Tokenizer::
Tokenizer(const Filepath& path)
{

    // Set the path.
    this->path = path;

    // Ensures the file is loaded through the global resource manager.
    ResourceManager& resmanager = ApplicationParameters::get().get_resource_manager();
    ResourceHandle handle = resmanager.create_resource_handle(path);
    SF_ASSERT(resmanager.resource_handle_is_valid(handle));
    resmanager.load_synchronously(handle);
    this->resource = handle;
    this->source = resmanager.get_resource_as_text(this->resource);

    // Set our token buffers.
    this->previous_token    = &token_buffer[0];
    this->current_token     = &token_buffer[1];
    this->next_token        = &token_buffer[2];

    // Valid initialize our tokens to EOF such that they're known values.
    for (i32 i = 0; i < 3; ++i)
    {
        this->token_buffer[i].type      = TokenType::TOKEN_EOF;
        this->token_buffer[i].resource  = this->resource;
        this->token_buffer[i].offset    = 0;
        this->token_buffer[i].length    = 0;
    }

    // This will prime the current and next tokens automatically for us.
    this->shift(); // Current token.
    this->shift(); // Next token.

}

Tokenizer::
~Tokenizer()
{



}

char Tokenizer::
consume(u32 count)
{

    char result = this->source[this->step + count - 1];
    this->step += count;
    return result;

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
    std::string identifier = this->next_token->to_string();
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
        while (this->peek(0) != '}' && !this->is_eof()) this->consume(1);

        // The comment could reach EOF, so account for that case and
        // generate the appropriate error token.
        if (this->is_eof())
        {

            this->next_token->type      = TokenType::TOKEN_UNDEFINED_EOF;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;

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
        this->next_token->type      = TokenType::TOKEN_NEW_LINE;
        this->next_token->resource  = this->resource;
        this->next_token->offset    = this->offset;
        this->next_token->length    = this->step - this->offset;
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

            this->next_token->type      = TokenType::TOKEN_UNDEFINED_EOF;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;

        }
        else
        {

            this->consume(1); // Consume trailing '}', this isn't in the next token.
            this->next_token->type      = TokenType::TOKEN_COMMENT_BLOCK;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;

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
            this->next_token->type      = TokenType::TOKEN_LEFT_PARENTHESIS;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case ')':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_RIGHT_PARENTHESIS;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case ',':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_COMMA;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;
        
        case ';':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_SEMICOLON;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '+':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_PLUS;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '-':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_MINUS;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;
        
        case '*':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_STAR;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '/':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_FORWARD_SLASH;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '^':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_CARROT;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '=':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_EQUALS;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '#':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_HASH;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;
        
        case '&':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_AMPERSAND;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '|':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_PIPE;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '%':
        {
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_PERCENT;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;
        } break;

        case '<':
        {
            char follower = this->peek(1);

            if (follower == '=')
            {
                this->consume(2);
                this->next_token->type      = TokenType::TOKEN_LESS_THAN_EQUALS;
                this->next_token->resource  = this->resource;
                this->next_token->offset    = this->offset;
                this->next_token->length    = this->step - this->offset;
                this->synchronize();
                return true;
            }
            
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_LESS_THAN;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;

        } break;

        case '>':
        {
            char follower = this->peek(1);

            if (follower == '=')
            {
                this->consume(2);
                this->next_token->type      = TokenType::TOKEN_GREATER_THAN_EQUALS;
                this->next_token->resource  = this->resource;
                this->next_token->offset    = this->offset;
                this->next_token->length    = this->step - this->offset;
                this->synchronize();
                return true;
            }
            
            this->consume(1);
            this->next_token->type      = TokenType::TOKEN_GREATER_THAN;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
            this->synchronize();
            return true;

        } break;

        case ':':
        {
            
            char follower = this->peek(1);
            if (follower == '=')
            {
                this->consume(2);
                this->next_token->type      = TokenType::TOKEN_COLON_EQUALS;
                this->next_token->resource  = this->resource;
                this->next_token->offset    = this->offset;
                this->next_token->length    = this->step - this->offset;
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

        this->next_token->type      = type;
        this->next_token->resource  = this->resource;
        this->next_token->offset    = this->offset;
        this->next_token->length    = this->step - this->offset;
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
            this->next_token->type      = TokenType::TOKEN_UNDEFINED_EOF;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
        }

        else if (this->is_eol())
        {
            this->next_token->type      = TokenType::TOKEN_UNDEFINED_EOL;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
        }

        else
        {

            this->next_token->type      = TokenType::TOKEN_STRING;
            this->next_token->resource  = this->resource;
            this->next_token->offset    = this->offset;
            this->next_token->length    = this->step - this->offset;
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

        this->next_token->type      = TokenType::TOKEN_IDENTIFIER;
        this->next_token->resource  = this->resource;
        this->next_token->offset    = this->offset;
        this->next_token->length    = this->step - this->offset;

        // Convert identifiers to keywords if they're keywords.
        this->next_token->type      = this->check_identifier();

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
        this->next_token->type      = TokenType::TOKEN_EOF;
        this->next_token->resource  = this->resource;
        this->next_token->offset    = this->offset;
        this->next_token->length    = 0;
        return;
    }

    // Match to specification.
    if (this->match_symbols())      return;
    if (this->match_numbers())      return;
    if (this->match_strings())      return;
    if (this->match_identifiers())  return;
    
    // If we're here, we didn't match to specification, the token is undefined.
    this->consume(1);
    this->next_token->type      = TokenType::TOKEN_UNDEFINED;
    this->next_token->resource  = this->resource;
    this->next_token->offset    = this->offset;
    this->next_token->length    = this->step - this->offset;
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
