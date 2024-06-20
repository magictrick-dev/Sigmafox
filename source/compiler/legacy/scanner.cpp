#include <compiler/scanner.h>

Scanner::
Scanner(const char *source)
{
    this->source = source;
}

char Scanner::
advance()
{
    char current_char = this->source[this->step++];
    return current_char;
}

char Scanner::
peek()
{
    char current_char = this->source[this->step];
    return current_char;
}

bool Scanner::
is_eof()
{
    return (this->source[this->step] == '\0' || this->source[this->step] == '\13' ||
            this->source[this->step] == '\10');
}

bool Scanner::
is_linecontrol()
{
    return (this->source[this->step] == '\r' || this->source[this->step] == '\n');
}

bool Scanner::
is_alpha()
{
    return (isalpha(this->source[this->step]));
}

bool Scanner::
is_numeric()
{
    return (this->source[this->step] >= '0' && this->source[this->step] <= '9');
}

bool Scanner::
is_alphanumeric()
{
    return (this->is_alpha() || this->is_numeric());
}

size_t Scanner::
last_line()
{
    return this->line;
}

bool Scanner::
error_was_eof_or_eol()
{
    return this->hit_eof;
}

void Scanner::
add_token(TokenType type)
{

    std::string lexeme = "";
    for (size_t i = this->start; i < this->step; ++i)
        lexeme += this->source[i];

    // Check if keywork, and if it is, modify identifier
    // to that keywork.
    if (type == TokenType::IDENTIFIER)
        type = identifier_to_token_type(lexeme);

    Token token_instance    = {};
    token_instance.type     = type;
    token_instance.lexeme   = lexeme;
    token_instance.line     = this->line;

    this->tokens.push_back(token_instance);

}

void Scanner::
add_error()
{

    std::string lexeme = "";
    for (size_t i = this->start; i < this->step; ++i)
        lexeme += this->source[i];

    Token token_instance    = {};
    token_instance.type     = TokenType::UNDEFINED;
    token_instance.lexeme   = lexeme;
    token_instance.line     = this->line;

    this->invalid_tokens.push_back(token_instance);

}

bool Scanner::
scan()
{

    this->step      = 0;
    this->start     = 0;
    this->line      = 1;
    this->hit_eof   = false;

    while (!this->is_eof())
    {

        char c = this->advance();              
        switch (c)
        {

            case '{':
            {
                while (this->peek() != '}' && !this->is_eof())
                    this->advance();

                if (this->is_eof())
                {
                    this->hit_eof = true;
                    return false;
                }

                this->start++;
                this->add_token(TokenType::COMMENT_BLOCK);
                this->advance(); // Skip the trailing '}'.
                break;
            };

            case '(': this->add_token(TokenType::LEFT_PARENTHESIS);     break;
            case ')': this->add_token(TokenType::RIGHT_PARENTHESIS);    break;
            case ';': this->add_token(TokenType::SEMICOLON);            break;
            case '+': this->add_token(TokenType::PLUS);                 break;
            case '-': this->add_token(TokenType::MINUS);                break;
            case '*': this->add_token(TokenType::MULTIPLY);             break;
            case '/': this->add_token(TokenType::DIVISION);             break;
            case '^': this->add_token(TokenType::POWER);                break;
            case '=': this->add_token(TokenType::EQUALS);               break;
            case '#': this->add_token(TokenType::NOTEQUALS);            break;
            case '&': this->add_token(TokenType::CONCAT);               break;
            case '|': this->add_token(TokenType::EXTRACT);              break;
            case '%': this->add_token(TokenType::DERIVATION);           break;
            
            // We skip white space.
            case '\t':
            case '\r':
            case ' ':
            {
                break;
            }

            case '\n': 
            {
                this->line++; 
                break;
            }

            case '\'':
            {
                
                
                while (this->peek() != '\'' && !this->is_linecontrol() && !this->is_eof())
                    this->advance();
                
                if (this->is_linecontrol() || this->is_eof())
                {
                    this->hit_eof = true;
                    return false;
                }

                this->start++;
                this->add_token(TokenType::STRING);
                this->advance(); // Skip the trailing '\''.
                break;
            };

            case '<': 
            {
                if (this->peek() == '=')
                {
                    this->advance();
                    this->add_token(TokenType::LESSTHANEQUALS);
                }
                else
                {
                    this->add_token(TokenType::LESSTHAN);
                }
                break;
            }
            case '>': 
            {
                if (this->peek() == '=')
                {
                    this->advance();
                    this->add_token(TokenType::GREATERTHANEQUALS);
                }
                else
                {
                    this->add_token(TokenType::GREATERTHAN);
                }
                break;
            }

            case ':':
            {
                if (this->peek() == '=')
                {
                    this->advance();
                    this->add_token(TokenType::ASSIGNMENT);
                    break;
                }
            }

            default:
            {

                if (isdigit(c))
                {
                    bool is_number = true;
                    while (c = this->peek())
                    {
                        if (isdigit(c)) this->advance();
                        else if (c == '.')
                        {
                            this->advance();
                            if (!isdigit(this->peek()))
                            {
                                is_number = false;
                                break;
                            }
                        }
                        else break;
                    }
                    if (is_number == true)
                    {
                        this->add_token(TokenType::NUMBER);
                        break;
                    }
                }

                else if (isalpha(c))
                {
                    
                    while (isalnum(c) || c == '_')
                    {
                        c = this->advance();
                    }
                    this->step -= 1;
                    this->add_token(TokenType::IDENTIFIER);
                    break;
                }

                this->add_error();

            };

        };

        this->start = this->step;

    }

    this->add_token(TokenType::ENDOFFILE);

    // If there are no invalid tokens, the scan was a success, return true.
    // Otherwise, return false if there are any invalid tokens.
    return (this->invalid_tokens.size() == 0);

}
