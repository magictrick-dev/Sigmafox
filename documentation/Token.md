# `Tokentype` Enum Documentation

The `Tokentype` enum defines various token types used for lexical analysis. These token types are used to categorize different components of the source code.

## Enum Values

- `TOKEN_VOID`: Represents a void token (typically used for type specifiers).
- `TOKEN_COMMENT_BLOCK`: Represents a block comment token.
- `TOKEN_LEFT_PARENTHESIS`: Represents a left parenthesis `(`.
- `TOKEN_RIGHT_PARENTHESIS`: Represents a right parenthesis `)`.
- `TOKEN_COMMA`: Represents a comma `,`.
- `TOKEN_SEMICOLON`: Represents a semicolon `;`.
- `TOKEN_COLON_EQUALS`: Represents the token `:=`.
- `TOKEN_PLUS`: Represents the plus sign `+`.
- `TOKEN_MINUS`: Represents the minus sign `-`.
- `TOKEN_STAR`: Represents the multiplication sign `*`.
- `TOKEN_FORWARD_SLASH`: Represents the forward slash `/`.
- `TOKEN_CARROT`: Represents the caret `^` (exponentiation operator).
- `TOKEN_EQUALS`: Represents the equals sign `=`.
- `TOKEN_LESS_THAN`: Represents the less than sign `<`.
- `TOKEN_LESS_THAN_EQUALS`: Represents the less than or equals sign `<=`.
- `TOKEN_GREATER_THAN`: Represents the greater than sign `>`.
- `TOKEN_GREATER_THAN_EQUALS`: Represents the greater than or equals sign `>=`.
- `TOKEN_HASH`: Represents the hash sign `#`.
- `TOKEN_AMPERSAND`: Represents the ampersand sign `&`.
- `TOKEN_PIPE`: Represents the pipe sign `|`.
- `TOKEN_PERCENT`: Represents the percent sign `%`.

- `TOKEN_INTEGER`: Represents an integer literal.
- `TOKEN_REAL`: Represents a real number (floating-point) literal.
- `TOKEN_COMPLEX`: Represents a complex number literal.
- `TOKEN_STRING`: Represents a string literal.
- `TOKEN_IDENTIFIER`: Represents an identifier (variable, function name, etc.).

### Keywords

- `TOKEN_KEYWORD_BEGIN`: Represents the `begin` keyword.
- `TOKEN_KEYWORD_ELSEIF`: Represents the `elseif` keyword.
- `TOKEN_KEYWORD_END`: Represents the `end` keyword.
- `TOKEN_KEYWORD_ENDFIT`: Represents the `endfit` keyword.
- `TOKEN_KEYWORD_ENDIF`: Represents the `endif` keyword.
- `TOKEN_KEYWORD_ENDFUNCTION`: Represents the `endfunction` keyword.
- `TOKEN_KEYWORD_ENDLOOP`: Represents the `endloop` keyword.
- `TOKEN_KEYWORD_ENDPLOOP`: Represents the `endploop` keyword.
- `TOKEN_KEYWORD_ENDPROCEDURE`: Represents the `endprocedure` keyword.
- `TOKEN_KEYWORD_ENDSCOPE`: Represents the `endscope` keyword.
- `TOKEN_KEYWORD_ENDWHILE`: Represents the `endwhile` keyword.
- `TOKEN_KEYWORD_FIT`: Represents the `fit` keyword.
- `TOKEN_KEYWORD_FUNCTION`: Represents the `function` keyword.
- `TOKEN_KEYWORD_IF`: Represents the `if` keyword.
- `TOKEN_KEYWORD_INCLUDE`: Represents the `include` keyword.
- `TOKEN_KEYWORD_LOOP`: Represents the `loop` keyword.
- `TOKEN_KEYWORD_PLOOP`: Represents the `ploop` keyword.
- `TOKEN_KEYWORD_PROCEDURE`: Represents the `procedure` keyword.
- `TOKEN_KEYWORD_READ`: Represents the `read` keyword.
- `TOKEN_KEYWORD_SAVE`: Represents the `save` keyword.
- `TOKEN_KEYWORD_SCOPE`: Represents the `scope` keyword.
- `TOKEN_KEYWORD_VARIABLE`: Represents the `variable` keyword.
- `TOKEN_KEYWORD_WHILE`: Represents the `while` keyword.
- `TOKEN_KEYWORD_WRITE`: Represents the `write` keyword.

- `TOKEN_NEW_LINE`: Represents a new line token.

### Special Tokens

- `TOKEN_EOF`: Represents the end of file (EOF) token with a value of 1000.
- `TOKEN_UNDEFINED`: Represents an undefined token.
- `TOKEN_UNDEFINED_EOF`: Represents an undefined EOF token.
- `TOKEN_UNDEFINED_EOL`: Represents an undefined end of line token.

# `Token` Struct Documentation

The `Token` struct represents a single token in the lexical analysis phase. It stores information about the token's reference, type, and its position in the source code.

## Struct Fields

- `reference` (std::string): The string representation of the token's value.
- `type` (Tokentype): The type of the token, as defined by the `Tokentype` enum.
- `row` (i32): The row number where the token was found in the source code.
- `column` (i32): The column number where the token was found in the source code.

## Static Member Functions

### `static std::string type_to_string(Tokentype type)`

Converts a `Tokentype` enum value to its string representation.

#### Parameters:
- `type` (Tokentype): The token type to be converted.

#### Returns:
- A `std::string` representing the token type.

