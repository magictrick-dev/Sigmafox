#include <lexer.h>
#include <sstream>
#include <iostream>

// --- Token -------------------------------------------------------------------
//
// Tokens contain lexeme information relative to the source file they're attached
// to and thus are only referencable from the lexer it is derived from.
//

Token::
Token(Lexeme lexeme, TokenType type)
    : lexeme(lexeme), type(type)
{ }

std::string Token::
to_string() const
{
    std::string str;
    for (size_t i = 0; i < this->lexeme.length; ++i)
        str += this->lexeme.reference[i + this->lexeme.offset];
    return str;
}

size_t Token::
get_offset() const
{
    return this->lexeme.offset;
};

size_t Token::
get_length() const
{
    return this->lexeme.length;
};

size_t Token::
get_line() const
{
    return this->lexeme.line_number;
}

size_t Token::
get_column() const
{
    return this->lexeme.column_number;
}

// --- Lexer -------------------------------------------------------------------
//
// Contains lexer functionality. A lexer object is ran per-source and contains
// a collection of tokens that it parses.
//

Lex::
Lex(const char *source, const char *path)
    : source(source)
{

    // Initialize step.
    this->step = 0;
    this->line_number = 1;
    this->line_offset = 0;
 
    // Get the full path and store it.
    sigmafox_file_get_full_path(path, this->path, 260);

    // Determine the length of the source.
    this->source_length = 0;
    while (this->source[this->source_length] != '\0')
        this->source_length++;

    // Initialize to no errors.
    this->has_error = false;

    // Start the parse.
    this->parse();

};

bool Lex::
has_errors() const
{
    return this->has_error;
}

void Lex::
print_errors() const
{
    for (const auto error : this->errors)
        std::cout << error << std::endl;
}

void Lex::
print_tokens() const
{
    for (const auto token : this->tokens)
    {
        std::cout   << "    Token: " << token.to_string() << " at position (" << token.get_line()
                    << "," << token.get_column() << ")" << std::endl;
    }
}

bool Lex::
is_eof() const
{
    return (this->source[this->step] == '\0' || this->source[this->step] == '\13' ||
            this->source[this->step] == '\10');
}

char Lex::
advance()
{
    char c = this->source[step++];
    return c;
}

char Lex::
peek()
{
    char c = this->source[step + 1];
    return c;
}

void Lex::
add_token(size_t offset, size_t length, TokenType type)
{
    size_t column = offset - this->line_offset + 1;
    Lexeme lexeme = { this->source, offset, length, this->line_number, column };
    this->tokens.push_back({ lexeme, type });
    return;
};

void Lex::
push_error(Token token)
{

    // TODO(Chris): We want our error formatting to match something like this, since it is
    //              a bit more robust in determining the file and such. The lexer will
    //              need more information about the file it is operating on, so we will
    //              need to adjust the constructor to take that.
    // C:\Development\Sigmafox\source\lexer.cpp(167,13): error C2143: syntax error: missing ';' before '}'

    std::stringstream ss = {};
    ss  << "  " << this->path << "(" << token.get_line() << "," << token.get_column()
        << "): Error, unrecognized symbol: " << token.to_string();

    this->errors.push_back(ss.str());
    this->has_error = true;
    return;

};

void Lex::
parse()
{

    // Essentially, step through everything in the file.
    while (!this->is_eof())
    {

        char c = this->advance();
        switch(c)
        {
            case '{':
            {
                this->add_token(this->step - 1, 1, TokenType::LEFT_CURLY_BRACKET);
                break;
            };

            case '}':
            {
                this->add_token(this->step - 1, 1, TokenType::RIGHT_CURLY_BRACKET);
                break;
            };

            case ';':
            {
                this->add_token(this->step - 1, 1, TokenType::SEMICOLON);
                break;
            }

            case '+':
            {
                this->add_token(this->step - 1, 1, TokenType::PLUS);
                break;
            };

            case '-':
            {
                this->add_token(this->step - 1, 1, TokenType::MINUS);
                break;
            };


            case '*':
            {
                this->add_token(this->step - 1, 1, TokenType::MULTIPLY);
                break;
            };

            case '/':
            {
                this->add_token(this->step - 1, 1, TokenType::DIVISION);
                break;
            };

            case '<':
            {
                this->add_token(this->step - 1, 1, TokenType::LESSTHAN);
                break;
            };

            case '>':
            {
                this->add_token(this->step - 1, 1, TokenType::GREATERTHAN);
                break;
            };

            case '=':
            {
                this->add_token(this->step - 1, 1, TokenType::EQUALS);
                break;
            };

            case '#':
            {
                this->add_token(this->step - 1, 1, TokenType::NOTEQUALS);
                break;
            };

            case '&':
            {
                this->add_token(this->step - 1, 1, TokenType::CONCAT);
                break;
            };

            case '|':
            {
                this->add_token(this->step - 1, 1, TokenType::EXTRACT);
                break;
            };

            case '%':
            {
                this->add_token(this->step - 1, 1, TokenType::DERIVATION);
                break;
            };

            // Whitespace conditions, we can skip these.
            case ' ':
            case '\r':
            case '\n':
            {

                if (c == '\n')
                {
                    this->line_number++;
                    this->line_offset = this->step;
                }

                break;
            }

            default:
            {
                this->add_token(this->step - 1, 1, TokenType::UNDEFINED);
                Token error_token = this->tokens.back();
                this->push_error(error_token);
                break;

            };
        };

    }

};
