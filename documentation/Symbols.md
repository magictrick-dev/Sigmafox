# Symbol Class Documentation

Represents a symbol in the program, such as a variable, function, or procedure.

---

## Enum: `Symboltype`

Defines the kind of symbol.

- `SYMBOL_TYPE_UNKNOWN` – Default or unclassified symbol.
- `SYMBOL_TYPE_DECLARED` – Symbol that has been declared but not yet defined.
- `SYMBOL_TYPE_VARIABLE` – A variable symbol.
- `SYMBOL_TYPE_PROCEDURE` – A procedure symbol.
- `SYMBOL_TYPE_FUNCTION` – A function symbol.

---

## Class: `Symbol`

Represents a named symbol with type, optional syntax reference, and arity (for arrays/functions).

### Constructors
- `Symbol()` – Default constructor.
- `Symbol(string name, Symboltype type, SyntaxNode* node, i32 arity = 0)` – Constructs a symbol with the given name, type, syntax node, and optional arity.

### Destructor
- `~Symbol()` – Virtual destructor.

### Setters
- `void set_name(string name)` – Sets the symbol's name.
- `void set_type(Symboltype type)` – Sets the symbol's type.
- `void set_node(SyntaxNode* node)` – Associates a syntax node with the symbol.
- `void set_arity(i32 arity)` – Sets the arity (e.g. number of dimensions for arrays).

### Getters
- `string get_name() const` – Returns the symbol's name.
- `Symboltype get_type() const` – Returns the symbol's type.
- `SyntaxNode* get_node() const` – Returns the associated syntax node.
- `i32 get_arity() const` – Returns the symbol's arity.

### Other Methods
- `bool is_array() const` – Returns `true` if the symbol represents an array (arity > 0).

### Protected Members
- `i32 arity` – Arity of the symbol, used for arrays/functions.
- `string name` – Name of the symbol.
- `Symboltype type` – Type of the symbol.
- `SyntaxNode* node` – Associated syntax node (e.g. AST reference).

