# Environment Class Documentation

The `Environment` class serves as the central context for the compiler. It is responsible for 
managing the symbol table stack, tokenizers, and validation routines during the parsing phase.
It serves as the backbone of the compiler's state and contains all the shared state needed
to ensure that the compiler works properly.

The environment keeps track of the symbol table stack, which gets pushed and popped from
as the parser nests scopes. The responsibility of the environment is to actually allow
for querying of currently visible variables during parsing *and* validation cycles that
are found in `compiler/parser/validators`. Specifically, the `BlockValidator` routine will
generate temporary scopes in order to simulate function and procedure calls so that the
internal variable definitions get updated when they're called.

---

## Overview

- Symbol tables are managed as a **stack**, where:
  - The **bottom** is the global scope.
  - New scopes push new tables.
  - The global table **cannot** be popped.
  - `pop_table()` returns `false` if popping the global table is attempted (asserts internally).
  
- Symbol lookup functions:
  - `symbol_exists()` checks all scopes.
  - `symbol_exists_locally()` checks the topmost (local) scope.
  - `symbol_exists_globally()` checks only the bottom (global) scope.
  - `symbol_exists_but_not_locally()` checks for symbols that exist in some parent scope but not locally.

---

## Class: `Environment`

### Constructors / Destructors
- `Environment()` – Initializes the environment with a global symbol table.
- `~Environment()` – Virtual destructor.

### Symbol Table Access
- `Symboltable& get_local_table()` – Returns the current top (local) symbol table.
- `Symboltable& get_global_table()` – Returns the global (bottom) symbol table.
- `void push_table()` – Pushes a new symbol table onto the stack (new scope).
- `bool pop_table()` – Pops the current symbol table from the stack. Returns `false` if the global table is attempted to be popped.

### Symbol Queries
- `bool symbol_exists(string identifier)` – Checks if a symbol exists in any scope.
- `bool symbol_exists_locally(string identifier)` – Checks only the current (local) scope.
- `bool symbol_exists_globally(string identifier)` – Checks only the global scope.
- `bool symbol_exists_but_not_locally(string identifier)` – True if the symbol exists globally or in parent scopes, but not locally.

### Symbol Access
- `Symbol* get_symbol(string identifier)` – Retrieves a symbol from any scope.
- `Symbol* get_symbol_locally(string identifier)` – Retrieves a symbol from the local scope.
- `Symbol* get_symbol_globally(string identifier)` – Retrieves a symbol from the global scope.

### Symbol Insertion
- `void set_symbol_locally(string identifier, Symbol symbol)` – Inserts or updates a symbol in the local scope.
- `void set_symbol_globally(string identifier, Symbol symbol)` – Inserts or updates a symbol in the global scope.

### Parsing State & Validation
- `bool is_begin_defined() const` – Returns `true` if the `BEGIN` token has been defined.
- `bool is_valid_parse() const` – Returns `true` if parsing is currently valid.
- `void define_begin()` – Marks the `BEGIN` section as defined.

### Error Handling
- `bool handle_compiler_exception(CompilerException& e)` – Handles a compiler exception; returns success/failure.

---

### Protected Members
- `bool warnings_as_errors` – If true, treats warnings as errors.
- `bool pedantic_output` – Enables stricter output behavior.
- `bool valid_parse` – Indicates the current parse validity state.
- `bool begin_defined` – Tracks whether `BEGIN` was encountered.
- `vector<Symboltable> tables` – Stack of symbol tables for scope management.


