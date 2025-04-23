# `Compiler` Class Documentation

The `Compiler` class is responsible for parsing, validating, and generating output based on the given input file. 
It interacts with a `DependencyGraph`, an `Environment`, and `SyntaxNode` elements to process and analyze the code structure.
The main purpose of this class is to centralize all the required components that the compiler needs to
operate. Specifically, `Environment` is of note due to many internal facilities relying on it. Source
file resolution occurs in the `DependencyGraph` class and is used by the parser to ensure that there
are only unique inclusions as well as detecting circular dependencies.

## Constructor

### `Compiler(string entry_file)`

The constructor initializes a `Compiler` object with the given entry file.

#### Parameters:
- `entry_file` (string): The path to the entry file that will be processed by the compiler.

## Destructor

### `~Compiler()`

The destructor cleans up resources used by the `Compiler` object.

## Member Functions

### `bool parse(bool show_reference = false)`

This function parses the provided entry file and constructs the syntax tree.

#### Parameters:
- `show_reference` (bool, optional): A flag indicating whether to display reference information during parsing. Defaults to `false`.

#### Returns:
- `true` if parsing is successful, `false` otherwise.

### `bool validate() const`

Validates the compiled structure. This includes syntax and semantic checks based on the `DependencyGraph` and `Environment`.

#### Returns:
- `true` if the validation is successful, `false` otherwise.

### `bool generate() const`

Generates the final output based on the parsed and validated data.

#### Returns:
- `true` if the generation is successful, `false` otherwise.

## Member Variables

### `DependencyGraph graph`

A graph structure that represents the dependencies between different nodes in the code.

### `Environment environment`

The environment in which the compiler operates. This holds configurations, settings, or runtime data relevant to the compilation process.

### `SyntaxNode* root`

The root node of the syntax tree. It represents the top-level structure of the parsed code.

### `std::vector<shared_ptr<SyntaxNode>> nodes`

A collection of syntax nodes that represent the parsed code. Each node is represented as a shared pointer to `SyntaxNode`.

