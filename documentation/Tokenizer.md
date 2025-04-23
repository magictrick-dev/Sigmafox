# `Tokenizer` Class Documentation

The `Tokenizer` class is responsible for lexical analysis of a source file. It reads the file and converts it into a stream of higher-order `Token` objects for further processing by the AST parser. 

The tokenizer maintains a shifting buffer consisting of three tokens: **previous**, **current**, and **next**, which facilitates contextual parsing (e.g., lookahead or lookbehind operations).

---

## Constructor

### `Tokenizer(const Filepath& path)`

Constructs a tokenizer and loads the source file at the given path for parsing.

#### Parameters:
- `path` (`Filepath`): Path to the source file to tokenize.

---

## Destructor

### `~Tokenizer()`

Cleans up tokenizer resources.

---

## Public Member Functions

### `void shift()`

Advances the tokenizer by one token. Updates the previous, current, and next token pointers accordingly.

### `Token get_previous_token() const`

Returns the previous token in the token stream.

### `Token get_current_token() const`

Returns the current token.

### `Token get_next_token() const`

Returns the next token in the stream.

### `b32 previous_token_is(Tokentype type) const`

Checks if the previous token is of a given type.

#### Parameters:
- `type` (`Tokentype`): The token type to check.

#### Returns:
- `true` if the previous token matches the type, otherwise `false`.

### `b32 current_token_is(Tokentype type) const`

Checks if the current token is of a given type.

### `b32 next_token_is(Tokentype type) const`

Checks if the next token is of a given type.

---

## Protected Member Functions

### `template <typename... Args> b32 match_set_of_characters(Args... args)`

Attempts to match a character in a provided set of arguments.

### `char peek(u32 look_ahead)`

Peeks at a character `look_ahead` positions ahead in the source buffer without consuming it.

### `char consume(u32 count)`

Consumes the next `count` characters from the source buffer.

### `b32 consume_whitespace()`

Consumes and skips over whitespace characters.

### `void synchronize()`

Synchronizes the token buffer with the current position in the source buffer.

### `Tokentype check_identifier() const`

Checks and classifies a token as an identifier or a keyword.

### `b32 is_eof() const`

Checks if the tokenizer has reached the end of the file.

### `b32 is_eol() const`

Checks if the tokenizer has reached the end of a line.

### `b32 match_newline()`

Matches newline characters in the source.

### `b32 match_comments()`

Matches comment tokens.

### `b32 match_symbols()`

Matches symbol tokens (e.g., `+`, `-`, `=`, etc.).

### `b32 match_numbers()`

Matches numeric tokens (integer, real, complex).

### `b32 match_strings()`

Matches string literal tokens.

### `b32 match_identifiers()`

Matches identifiers or keywords.

### `void set_token(Token* token, Tokentype type)`

Sets the type for a token in the buffer.

---

## Member Variables

### File Handling

- `Filepath path`: The file path of the source being tokenized.
- `std::string source`: The raw source code content.

### Token Buffer

- `Token token_buffer[3]`: Circular buffer to hold previous, current, and next tokens.
- `Token* previous_token`: Pointer to the previous token.
- `Token* current_token`: Pointer to the current token.
- `Token* next_token`: Pointer to the next token.

### Position Tracking

- `u64 row`: Current row number in the source (starts at 1).
- `u64 column`: Current column number in the source (starts at 1).
- `u64 offset`: Byte offset in the source string.
- `u64 step`: Number of tokens processed (or tokenizer steps taken).

---

