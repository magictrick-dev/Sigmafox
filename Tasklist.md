# Sigmafox Tasklist (December 2024)

### Project Outline & Target Functionality 

Sigmafox is a multifaceted development suite. There are two primary components: first
is the language transpiler that transforms `.fox` scripts to C++, and the second being
the core support libraries in C++ required for automatic differentiation, cluster computation,
dynamic typing, and I/O functionality.

- [ ] **CLI Parser**

    The CLI parser is responsible for taking the transpiler's CLI arguments and modifying
    the runtime parameters of the transpiler for the user. The runtime parameters have set
    defaults. These defaults should be defined for the typical user-experience.

- [ ] **Platform Environment**

    The platform environment is the platform-specific abstraction APIs that the rest of the
    program uses. Rather than relying on the STL for platform abstractions, this API is designed
    to be robust and fast. The obvious trade-off is that every platform must be defined and tested.
    Given the limited set of environments (Windows, MacOSX, and Linux) this should not require
    significant overhead to maintain.

- [ ] **Source File Tokenizer**

    The source file tokenizer primarily consumes text from `.fox` files and returns abstractions
    called tokens. The parser takes these tokens and operates on them rather than the raw-text of
    the source file.

- [ ] **AST Language Parser**

    The AST parser defines the essential language grammar for Sigmafox. The parser is the most
    complex portion of the transpiler and requires great care to ensure consistent functionality
    with the language specification. The parser either returns a valid abstract syntax tree on
    valid parse.

- [ ] **Error Handler**

    The error handler provides well-defined error messages for the user if there exists a problem
    parsing code. Generally, the user will want to know the file name, line, and column number of
    where the error has occured.

- [ ] **Symbol Table**

    The symbol table determines whether or not a particular symbol is declared or defined. The
    symbol table functions a lot like a stack in that entrant scopes push a new table, and exit
    scopes pop the table. Symbol lookup can occur as granular as the current scope, or all scopes.

- [ ] **File Generation**

    File generation occurs after parsing and converts the AST into valid C++.

### Refactoring to C++

The project was originally written in C. In order to make it easier for future maintainers, a complete
refactor (back) to C++ will be the best route going forward. Since the core architecture is well-understood,
such a refactor will allow for the correction of accrued technical debt.

1.  Refactor the CLI argument interface. The CLI interface should have access to a global singleton class
    `ApplicationParameters` that the rest of the application can query. An additional singleton class called
    `ApplicationStatistics` should also be available to track and log information. Logging information should
    be written to disk and placed into the invocation directory.

2.  Platform environment should be developed for file I/O access as well as directory control functionality.
    A push to have Linux compatibility will be emphasized and therefore this will be developed hand-in-hand
    with Windows. MacOSX is POSIX-like, but I don't have access to a Mac to develop and test on. File pathing
    should be absolute and therefore we will need a library to construct paths relative to the current working
    directory (the "project directory") and the executable directory (where we will have core libraries stored).
    This is desirable because we want the compiler to be located in a static location and included as system path.

    We will need `Filepath` class which stores the strings of paths, as well as some necessary file validation
    utilities to help ensure paths are valid. From there, we want a `ResourceManager` which contains in-memory
    `Resource` objects. The tokenizer is the primary end-point for resources.