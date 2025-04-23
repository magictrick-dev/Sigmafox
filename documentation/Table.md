# Symboltable Class Documentation

Represents a symbol table that maps string identifiers to `Symbol` instances.

---

## Class: `Symboltable`

A hash-based container for storing and retrieving symbols by name.

### Constructors
- `Symboltable()` – Default constructor.
- `~Symboltable()` – Virtual destructor.

### Public Methods
- `bool exists(string key) const`  
  Returns `true` if a symbol with the given key exists in the table.

- `bool insert(Symbol symbol)`  
  Inserts a new symbol into the table. Returns `true` on success, `false` if the key already exists.

- `Symbol* find(string key)`  
  Retrieves a pointer to the symbol with the given key, or `nullptr` if not found.

### Protected Members
- `std::unordered_map<std::string, Symbol> symbols`  
  Internal map storing symbol entries by their names.


