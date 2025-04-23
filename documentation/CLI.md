# CLI Parsing System

This system provides a class-based interface for parsing command-line arguments in a structured and extensible manner. The core components include `CLIArgument` and its derived types for different argument forms, and the `CLI` class which handles the parsing and storage of all arguments.

---


### `CLIArgument`
Base class representing a generic command-line argument.

**Members:**
- `get_index()`: Returns the original index of the argument.
- `get_argument()`: Returns the raw argument string.
- `get_type()`: Returns the type of the argument (`CLIArgumentType`).
- `set_type(CLIArgumentType type)`: Sets the type of the argument.

**Protected Members:**
- `type`: Argument type (`Error`, `Switch`, `Parameter`, `Real`, `Numeric`, `String`).
- `raw_index`: Index of the argument.
- `raw_argument`: Raw C-style string value of the argument.

---

### `CLIValue : CLIArgument`
Represents non-switch and non-parameter values. Also acts as a fallback for errors.

**Static Methods:**
- `parse(i32 index, ccptr argument)`: Parses a generic value.
- `error(i32 index, ccptr argument)`: Returns a CLIArgument representing an error.

**Getters:**
- `get_string()`: Returns the argument as a string.
- `get_real()`: Returns the value as a real number (`r64`).
- `get_signed()`: Returns the value as a signed integer (`i64`).
- `get_unsigned()`: Returns the value as an unsigned integer (`u64`).

---

### `CLIParameter : CLIArgument`
Represents parameters in the form of `--[name]`.

**Static Methods:**
- `parse(i32 index, ccptr argument)`: Parses a parameter argument.

**Members:**
- `get_name()`: Returns the name of the parameter.

---

### `CLISwitch : CLIArgument`
Represents switches in the form of `-[A-Za-z]*`.

**Static Methods:**
- `parse(i32 index, ccptr argument)`: Parses a switch argument.

**Operators:**
- `operator[](char c)`: Access switch flags via character.
- `operator[](i32 idx)`: Access switch flags via index.

---

### `CLI`
Main interface for command-line parsing and argument management.

**Static Methods:**
- `parse(i32 argc, cptr* argv)`: Parses all command-line arguments.
- `has_flag(char c)`: Checks if a specific switch is present.
- `has_parameter(ccptr parameter)`: Checks for a parameter by name.
- `get(i32 i)`: Returns the i-th parsed argument.
- `size()`: Returns the total number of parsed arguments.
- `header()`, `short_help()`, `long_help()`: Displays help messages.

---

## Enum `CLIArgumentType`
Defines the classification of arguments:
- `Error`: Unrecognized or invalid input.
- `Switch`: Short-form switch flags (`-x`, `-abc`).
- `Parameter`: Long-form named parameters (`--name`).
- `Real`: Parsed floating-point values.
- `Numeric`: Parsed integer values.
- `String`: Parsed string values.

# Symbol Class Documentation

Represents a symbol in the program, such as a variable, function, or procedure. Used primarily in symbol tables for parsing or compiling purposes.

## Enum: `Symboltype`

Defines the kind of symbol.

- `SYMBOL_TYPE_UNKNOWN` – Default or unclassified symbol.
- `SYMBOL_TYPE_DECLARED` – Symbol that has been declared but not yet defined.
- `SYMBOL_TYPE_VARIABLE` – A variable symbol.
- `SYMBOL_TYPE_PROCEDURE` – A procedure symbol.
- `SYMBOL_TYPE_FUNCTION` – A function symbol.

---

