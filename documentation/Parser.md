# `ParseTree` Class Documentation

The `ParseTree` class is responsible for constructing an Abstract Syntax Tree (AST) from a source file using a tokenizer. It utilizes a `DependencyGraph` and `Environment` to manage global context during parsing. The class supports various parsing routines for statements and expressions, and is a key part of the compilation pipeline.

---

## Constructor

### `ParseTree(DependencyGraph* graph, Environment* environment)`

Constructs a `ParseTree` with a given dependency graph and environment context.

#### Parameters:
- `graph` (`DependencyGraph*`): Pointer to the dependency graph used during parsing.
- `environment` (`Environment*`): Pointer to the environment configuration/context.

---

## Destructor

### `~ParseTree()`

Cleans up the `ParseTree` instance.

---

## Deleted Copy Operations

- `ParseTree(const ParseTree&) = delete`: Copy constructor is disabled.
- `ParseTree& operator=(const ParseTree&) = delete`: Copy assignment is disabled.

---

## Public Member Functions

### `bool parse(string source_file)`

Parses the given source file and constructs the AST.

#### Parameters:
- `source_file` (`string`): Path to the source file to parse.

#### Returns:
- `true` if parsing is successful, otherwise `false`.

### `bool valid() const`

Checks whether the constructed syntax tree is valid.

#### Returns:
- `true` if valid, otherwise `false`.

### `SyntaxNode* get_root()`

Returns a pointer to the root node of the syntax tree.

### `vector<shared_ptr<SyntaxNode>>& get_nodes()`

Returns a reference to the list of all parsed syntax nodes.

---

## Protected Synchronization Functions

### `void synchronize_to(Tokentype type)`

Advances the tokenizer until a token of the given type is found.

### `void synchronize_up_to(Tokentype type)`

Advances the tokenizer until a token of the given type is passed.

---

## Protected Token Expectation & Consumption

### `bool expect_current_token_as(Tokentype type) const`

Checks if the current token matches the given type.

### `bool expect_next_token_as(Tokentype type) const`

Checks if the next token matches the given type.

### `void consume_current_token_as(Tokentype type, u64 sloc)`

Consumes the current token if it matches the expected type; otherwise, raises an error at the specified source location.

---

## Template Node Generation

### `template <class T, typename ...Params> T* generate_node(Params... args)`

Generates a syntax node of type `T` with the provided arguments.

---

## Root Matching

### `SyntaxNode* match_root()`

Matches and returns the root node of the syntax tree.

---

## Statement Matching Functions

Each function below parses a specific kind of statement and returns a pointer to a `SyntaxNode` representing that statement.

- `match_global_statement()`
- `match_include_statement()`
- `match_function_statement(bool is_global)`
- `match_procedure_statement(bool is_global)`
- `match_begin_statement()`
- `match_local_statement()`
- `match_expression_statement()`
- `match_variable_statement()`
- `match_scope_statement()`
- `match_while_statement()`
- `match_loop_statement()`
- `match_ploop_statement()`
- `match_conditional_if_statement()`
- `match_conditional_elseif_statement()`
- `match_read_statement()`
- `match_write_statement()`

---

## Expression Matching Functions

Each of the following methods is responsible for parsing specific components of expressions:

- `match_expression()`
- `match_procedure_call()`
- `match_assignment()`
- `match_equality()`
- `match_comparison()`
- `match_concatenation()`
- `match_term()`
- `match_factor()`
- `match_magnitude()`
- `match_extraction()`
- `match_derivation()`
- `match_unary()`
- `match_function_call()`
- `match_array_index()`
- `match_primary()`

---

## Member Variables

- `DependencyGraph* graph`: Dependency graph used during parsing.
- `Environment* environment`: Pointer to the environment configuration.
- `shared_ptr<Tokenizer> tokenizer`: Tokenizer used to process source code.
- `Filepath path`: Path to the source file being parsed.
- `SyntaxNode* root`: Pointer to the root node of the constructed syntax tree.
- `vector<shared_ptr<SyntaxNode>> nodes`: Container for all syntax nodes generated during parsing.

---

