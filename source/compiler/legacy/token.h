#ifndef SIGMAFOX_COMPILER_TOKEN_H
#define SIGMAFOX_COMPILER_TOKEN_H
#include <string>
#include <unordered_map>

enum class TokenType
{

    // Symbols
    COMMENT_BLOCK,          // { ... }
    LEFT_PARENTHESIS,       // (
    RIGHT_PARENTHESIS,      // )
    SEMICOLON,              // ;
    ASSIGNMENT,             // :=
    PLUS,                   // +
    MINUS,                  // -
    MULTIPLY,               // *
    DIVISION,               // /
    POWER,                  // ^
    LESSTHAN,               // <
    GREATERTHAN,            // >
    LESSTHANEQUALS,         // <=
    GREATERTHANEQUALS,      // >=
    EQUALS,                 // =
    NOTEQUALS,              // #
    CONCAT,                 // &
    EXTRACT,                // |
    DERIVATION,             // %

    // Definables
    IDENTIFIER,
    STRING,                 // '', Single quotes only.
    NUMBER,

    // Keywords
    BEGIN,      
    END,
    PROCEDURE,  
    ENDPROCEDURE,
    FUNCTION,   
    ENDFUNCTION,
    IF,         
    ENDIF,
    WHILE,      
    ENDWHILE,
    LOOP,       
    ENDLOOP,
    PLOOP,      
    ENDPLOOP,
    FIT,        
    ENDFIT,
    VARIABLE,
    WRITE,
    READ,
    SAVE,
    INCLUDE,
    PRINT,

    // Unidentified
    UNDEFINED,
    ENDOFFILE,

};

inline std::string
token_type_to_string(TokenType type)
{
    switch (type)
    {
        case (TokenType::COMMENT_BLOCK):        return "COMMENT BLOCK";
        case (TokenType::LEFT_PARENTHESIS):     return "LEFT PARENTHESIS";
        case (TokenType::RIGHT_PARENTHESIS):    return "RIGHT PARENTHESIS";
        case (TokenType::SEMICOLON):            return "SEMICOLON";
        case (TokenType::ASSIGNMENT):           return "ASSIGNMENT";
        case (TokenType::PLUS):                 return "ADDITION";
        case (TokenType::MINUS):                return "SUBTRACTION";
        case (TokenType::MULTIPLY):             return "MULTIPLICATION";
        case (TokenType::DIVISION):             return "DIVISION";
        case (TokenType::POWER):                return "POWER";
        case (TokenType::LESSTHAN):             return "LESS THAN";
        case (TokenType::GREATERTHAN):          return "GREATER THAN";
        case (TokenType::GREATERTHANEQUALS):    return "GREATER THAN OR EQUAL TO";
        case (TokenType::LESSTHANEQUALS):       return "LESS THAN OR EQUAL TO";
        case (TokenType::EQUALS):               return "EQUALS";
        case (TokenType::NOTEQUALS):            return "NOT EQUALS";
        case (TokenType::CONCAT):               return "CONCAT";
        case (TokenType::EXTRACT):              return "EXTRACT";
        case (TokenType::DERIVATION):           return "DERIVATION";
        case (TokenType::IDENTIFIER):           return "IDENTIFIER";
        case (TokenType::STRING):               return "STRING";
        case (TokenType::NUMBER):               return "NUMBER";
        case (TokenType::BEGIN):                return "BEGIN";
        case (TokenType::END):                  return "END";
        case (TokenType::PROCEDURE):            return "PROCEDURE";
        case (TokenType::ENDPROCEDURE):         return "ENDPROCEDURE";
        case (TokenType::FUNCTION):             return "FUNCTION";
        case (TokenType::ENDFUNCTION):          return "ENDFUNCTION";
        case (TokenType::IF):                   return "IF";
        case (TokenType::ENDIF):                return "ENDIF";
        case (TokenType::WHILE):                return "WHILE";
        case (TokenType::ENDWHILE):             return "ENDWHILE";
        case (TokenType::LOOP):                 return "LOOP";
        case (TokenType::ENDLOOP):              return "ENDLOOP";
        case (TokenType::PLOOP):                return "PLOOP";
        case (TokenType::ENDPLOOP):             return "ENDPLOOP";
        case (TokenType::FIT):                  return "FIT";
        case (TokenType::ENDFIT):               return "ENDFIT";
        case (TokenType::VARIABLE):             return "VARIABLE";
        case (TokenType::WRITE):                return "WRITE";
        case (TokenType::READ):                 return "READ";
        case (TokenType::SAVE):                 return "SAVE";
        case (TokenType::INCLUDE):              return "INCLUDE";
        case (TokenType::PRINT):                return "PRINT";
        case (TokenType::ENDOFFILE):            return "EOF";
                                                
        default: return "UNDEFINED";
    };
}

inline TokenType 
identifier_to_token_type(std::string identifier)
{

    // The map won't change, so we store it statically here for
    // optimal performance.
    static std::unordered_map<std::string, TokenType> keyword_map =
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
        { "print",          TokenType::PRINT        },
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

class Token
{
    public:
        inline std::string to_string() const { return token_type_to_string(type) + " " + lexeme; };

        TokenType       type        = TokenType::UNDEFINED;
        std::string     lexeme      = "";
        size_t          line        = 1;

};

#endif
