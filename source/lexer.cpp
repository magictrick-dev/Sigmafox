#include <lexer.h>

#include <sstream>
#include <iostream>

// --- Helper Functions --------------------------------------------------------
//
// These are common helper functions that are used through the lexer.
//

SF_INTERNAL SF_INLINE size_t
sigmafox_lexer_line_number_from_offset(const char *source, size_t offset)
{

    size_t line_number = 1;
    for (size_t idx = 0; idx < offset; ++idx)
        if (source[idx] == '\n')
            line_number++;

    return line_number;

}

SF_INTERNAL SF_INLINE size_t
sigmafox_lexer_column_number_from_offset(const char *source, size_t offset)
{

    size_t last_line_position = 0;
    size_t line_number = 1;
    for (size_t idx = 0; idx < offset; ++idx)
    {
        if (source[idx] == '\n')
        {
            last_line_position = idx;
            line_number++;
        }
    }

    size_t column = offset - last_line_position + 1;
    return column;

}

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
    return sigmafox_lexer_line_number_from_offset(this->lexeme.reference, this->lexeme.offset);
}

size_t Token::
get_column() const
{
    return sigmafox_lexer_column_number_from_offset(this->lexeme.reference, this->lexeme.offset);
}

// --- Lexer -------------------------------------------------------------------
//
// Contains lexer functionality. A lexer object is ran per-source and contains
// a collection of tokens that it parses.
//

Lex::
Lex(const char *source)
    : source(source)
{

    // Initialize step.
    this->step = 0;
    
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
        std::cout   << "  Token: " << token.to_string() << " at position (" << token.get_line()
                    << "," << token.get_column() << ")" << std::endl;
    }
}

bool Lex::
is_eof() const
{
    
    return (this->source[this->step] == '\0' ||
            this->source[this->step] == 13   ||
            this->source[this->step] == 10);

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
add_token(Lexeme lexeme, TokenType type)
{
    this->tokens.push_back({ lexeme, type });
    return;
};

void Lex::
push_error(std::string error, size_t at)
{

    // TODO(Chris): We want our error formatting to match something like this, since it is
    //              a bit more robust in determining the file and such. The lexer will
    //              need more information about the file it is operating on, so we will
    //              need to adjust the constructor to take that.
    // C:\Development\Sigmafox\source\lexer.cpp(167,13): error C2143: syntax error: missing ';' before '}'

    // Calculate the relative position of the error.
    size_t line_number      = 1;
    size_t column_number    = 0;
    size_t last_line        = 0;

    for (size_t idx = 0; idx < at; ++idx)
    {
        if (this->source[idx] == '\n')
        {
            last_line = idx;
            line_number++;
        }
    }

    column_number = at - last_line;

    // Now that we know where the error is, we can display the line and point to
    // the error for a better user-experience.
    size_t line_length = 0;
    std::string line_contents;
    for (size_t i = 0; i < this->source_length; ++i)
    {
        if (this->source[line_length] == '\n' ||
            this->source[line_length] == '\r')
                break;
        line_contents += this->source[line_length++];
    }

    std::stringstream ss = {};
    ss  << "  Error on line " << line_number << ", column " << column_number
        << ": " << error << std::endl;

    ss  << "  See: " << line_contents << std::endl;
    ss  << "       ";
    for (size_t i = 0; i < column_number - 1; ++i)
        ss << " ";
    ss << "^";

    

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
                this->add_token({this->source, this->step - 1, 1}, TokenType::LEFT_CURLY_BRACKET);
                break;
            };

            case '}':
            {
                this->add_token({ this->source, this->step - 1, 1 }, TokenType::RIGHT_CURLY_BRACKET);
                break;
            };

            case ';':
            {
                this->add_token({ this->source, this->step - 1, 1 }, TokenType::SEMICOLON);
                break;
            }

            case '+':
            {
                this->add_token({ this->source, this->step - 1, 1 }, TokenType::PLUS);
                break;
            };

            case '-':
            {
                this->add_token({ this->source, this->step - 1, 1 }, TokenType::MINUS);
                break;
            };


            case '*':
            {
                this->add_token({ this->source, this->step - 1, 1 }, TokenType::MULTIPLY);
                break;
            };

            case '/':
            {
                this->add_token({ this->source, this->step - 1, 1 }, TokenType::DIVISION);
                break;
            };

            // Whitespace conditions, we can skip these.
            case ' ':
            case '\r':
            case '\n':
            {
                break;
            }

            default:
            {
                this->add_token({ this->source, this->step - 1, 1 }, TokenType::UNDEFINED);
                this->push_error("Undefined symbol encountered.", this->step);
                break;

            };
        };

    }

};
