#include <lexer.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>

std::string
token_type_to_string(TokenType type)
{

    switch (type)
    {
        case TokenType::LEFT_CURLY_BRACKET:     { return "left_curly";      }
        case TokenType::RIGHT_CURLY_BRACKET:    { return "right_curly";     }
        case TokenType::SEMICOLON:              { return "semicolon";       }
        case TokenType::ASSIGNMENT_OPERATOR:    { return "assignment";      }
        case TokenType::PLUS:                   { return "plus";            }
        case TokenType::MINUS:                  { return "minus";           }
        case TokenType::MULTIPLY:               { return "multiply";        }
        case TokenType::DIVISION:               { return "division";        }
        case TokenType::POWER:                  { return "power";           }
        case TokenType::LESSTHAN:               { return "lessthan";        }
        case TokenType::GREATERTHAN:            { return "greaterthan";     }
        case TokenType::EQUALS:                 { return "equals";          }
        case TokenType::NOTEQUALS:              { return "notequals";       }
        case TokenType::CONCAT:                 { return "concat";          }
        case TokenType::EXTRACT:                { return "extract";         }
        case TokenType::DERIVATION:             { return "derivation";      }

        case TokenType::IDENTIFIER:             { return "identifier";      }
        case TokenType::STRING_SINGLE:          { return "string_signle";   }
        case TokenType::STRING_DOUBLE:          { return "string_double";   }
        case TokenType::NUMBER:                 { return "number_literal";  }

        case TokenType::BEGIN:                  { return "begin";           }
        case TokenType::END:                    { return "end";             }
        case TokenType::PROCEDURE:              { return "procedure";       }
        case TokenType::ENDPROCEDURE:           { return "endprocedure";    }
        case TokenType::FUNCTION:               { return "function";        }
        case TokenType::ENDFUNCTION:            { return "endfunction";     }    
        case TokenType::IF:                     { return "if";              }
        case TokenType::ENDIF:                  { return "endif";           }
        case TokenType::WHILE:                  { return "while";           }
        case TokenType::ENDWHILE:               { return "endwhile";        }
        case TokenType::LOOP:                   { return "loop";            }
        case TokenType::ENDLOOP:                { return "endloop";         }
        case TokenType::PLOOP:                  { return "ploop";           }
        case TokenType::ENDPLOOP:               { return "endploop";        }
        case TokenType::FIT:                    { return "fit";             }
        case TokenType::ENDFIT:                 { return "endfit";          }
        case TokenType::VARIABLE:               { return "variable";        }
        case TokenType::WRITE:                  { return "write";           }
        case TokenType::READ:                   { return "read";            }
        case TokenType::SAVE:                   { return "save";            }
        case TokenType::INCLUDE:                { return "include";         }
        default:                                { return "undefined";       }

    }

    return "";

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
    return this->lexeme.line_number;
}

size_t Token::
get_column() const
{
    return this->lexeme.column_number;
}

TokenType Token::
get_type() const
{
    return this->type;
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
        std::stringstream header = {};
        header  << "  Token(" << token.get_line() << "," << token.get_column() << ","
                << token_type_to_string(token.get_type()) << "): ";
        std::cout << std::setw(34) << std::left << header.str() << token.to_string() << std::endl;
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
    char c = this->source[step];
    return c;
}

bool Lex::
is_number(char c)
{
    return (c >= '0' && c <= '9');
}

bool Lex::
is_alpha(char c)
{
    bool is_lower = (c >= 'a' && c <= 'z');
    bool is_upper = (c >= 'A' && c <= 'Z');
    return ( is_lower || is_upper );
}

bool Lex::
is_alphanumeric(char c)
{

    bool is_number = this->is_number(c);
    bool is_alpha = this->is_alpha(c);
    return ( is_number || is_alpha );

}

bool Lex::
is_linecontrol(char c)
{
    return (c == '\r' || c == '\n');
}

bool Lex::
is_eof(char c)
{
    return (this->source[this->step] == '\0' || this->source[this->step] == '\13' ||
            this->source[this->step] == '\10');
}

void Lex::
add_token(size_t offset, size_t length, TokenType type)
{
    size_t column = offset - this->line_offset + 1;
    Lexeme lexeme = { this->source, offset, length, this->line_number, column };
    this->tokens.push_back({ lexeme, type });
    return;
};

TokenType Lex::
check_keyword(std::string identifier)
{
    
    SF_PERSIST std::unordered_map<std::string, TokenType> keyword_map =
    {
        { "begin",          TokenType::BEGIN        },
        { "end",            TokenType::END          },
        { "procedure",      TokenType::PROCEDURE    },
        { "endprocedure",   TokenType::ENDPROCEDURE },
        { "function",       TokenType::FUNCTION     },
        { "endfunction",    TokenType::ENDFUNCTION  },
        { "if",             TokenType::IF           },
        { "endif",          TokenType::ENDIF        },
        { "while",          TokenType::WHILE        },
        { "endwhile",       TokenType::ENDWHILE     },
        { "loop",           TokenType::LOOP         },
        { "endloop",        TokenType::ENDLOOP      },
        { "ploop",          TokenType::PLOOP        },
        { "endploop",       TokenType::ENDPLOOP     },
        { "fit",            TokenType::FIT          },
        { "endfit",         TokenType::ENDFIT       },
        { "variable",       TokenType::VARIABLE     },
        { "write",          TokenType::WRITE        },
        { "read",           TokenType::READ         },
        { "save",           TokenType::SAVE         },
        { "include",        TokenType::INCLUDE      },
    };

    // Search for the string.
    for (char &c : identifier)
        c = std::tolower(c);
    auto location = keyword_map.find(identifier);

    // If the location is found, then return the type.
    if (location != keyword_map.end())
    {
        return location->second;
    }
    
    else
    {
        return TokenType::IDENTIFIER;
    }
}

void Lex::
push_error(Token token)
{

    std::stringstream ss = {};
    ss  << "  " << this->path << "(" << token.get_line() << "," << token.get_column()
        << "): Error, unrecognized symbol: \n  - Error contents: " << token.to_string();

    this->errors.push_back(ss.str());
    this->has_error = true;
    return;

};

void Lex::
push_error(Token token, std::string message)
{

    std::stringstream ss = {};
    ss  << "  " << this->path << "(" << token.get_line() << "," << token.get_column()
        << "): Error, " << message << ": \n  - Error contents: " << token.to_string();

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
                while (this->peek() != '}')
                    this->advance();
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

            case '\'':
            {
                
                // Move one passed the quote.
                i32 length = 1;
                char last;
                while (this->peek() != '\'' && 
                        !this->is_linecontrol(this->peek()) &&
                        !this->is_eof(this->peek()))
                {
                    length++;
                    this->advance();
                    last = this->peek();
                }
                
                if (this->is_linecontrol(last) || this->is_eof(last))
                {
                    this->add_token(this->step - length, length, TokenType::UNDEFINED);
                    Token error_token = this->tokens.back();
                    this->push_error(error_token, "single quote string reached end-of-line");
                }
                else
                {
                    this->advance();
                    length++;

                    this->add_token(this->step - length, length, TokenType::STRING_SINGLE);
                    this->advance();
                }

                break;

            };

            case '\"':
            {
                
                // Move one passed the quote.
                i32 length = 1;
                char last;
                while (this->peek() != '\"' && 
                        !this->is_linecontrol(this->peek()) &&
                        !this->is_eof(this->peek()))
                {
                    length++;
                    this->advance();
                    last = this->peek();
                }
                
                if (this->is_linecontrol(last) || this->is_eof(last))
                {
                    this->add_token(this->step - length, length, TokenType::UNDEFINED);
                    Token error_token = this->tokens.back();
                    this->push_error(error_token, "double quote string reached end-of-line");
                }
                else
                {
                    this->advance();
                    length++;

                    this->add_token(this->step - length, length, TokenType::STRING_SINGLE);
                    this->advance();
                }

                break;

            };

            case '*':
            {

                if (this->peek() == '*')
                {
                    this->add_token(this->step - 1, 2, TokenType::POWER);
                    this->advance();
                }
                else
                {
                    this->add_token(this->step - 1, 1, TokenType::MULTIPLY);
                }

                break;
            };

            case ':':
            {
                if (this->peek() == '=')
                {
                    this->add_token(this->step - 1, 2, TokenType::ASSIGNMENT_OPERATOR);
                    this->advance();
                    break;
                }
                else
                {
                    this->add_token(this->step - 1, 1, TokenType::UNDEFINED);
                    Token error_token = this->tokens.back();
                    this->push_error(error_token);
                    break;
                }
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

                // Numbers.
                if (this->is_number(c))
                {

                    i32 length = 1;
                    while (this->is_number(this->peek()))
                    {
                        length++;
                        this->advance();
                    }

                    this->add_token(this->step - length, length, TokenType::NUMBER);

                }

                // Identifiers.
                else if (this->is_alpha(c))
                {

                    i32 length = 1;
                    while (this->is_alphanumeric(this->peek()) ||
                            this->peek() == '_')
                    {
                       length++; 
                       this->advance();
                    }

                    // Convert to a string.
                    std::string identifier;
                    for (size_t i = this->step - length; i < this->step; i++)
                        identifier += this->source[i];
                    this->add_token(this->step - length, length, this->check_keyword(identifier));

                }

                // Anything else.
                else
                {

                    this->add_token(this->step - 1, 1, TokenType::UNDEFINED);
                    Token error_token = this->tokens.back();
                    this->push_error(error_token);
                    break;

                }

            };
        };

    }

};
