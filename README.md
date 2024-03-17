# POSY - Parser for COSY

A syntax parser and transpiler for COSY-scripts which produce executable
C++ source files.

## Building POSY

POSY requires CMake to build the project. Given that it is being run on
a Linux environment, the provided `build.sh` and `run.sh` will do the
heavy lifting for you. The `run.sh` script forwards all arguments to POSY,
or simply run POSY from the `./bin` folder.

## Using POSY

Invoke POSY like most CLI applications by providing a list of source files
to parse and any additional parameters (given they're supported).

```
POSY ./tests/basic.foxy
```

Given the parse was a success, an output file of the same base name will
be created, such as `basic.posy.cpp` along with any additional supporting
code needed to execute as well as an appropriate build file (either CMakeLists.txt
or a makefile) to easily compile the project.

## Project TODOs

- [ ] Language Lexer
    
    A language lexer is required to tokenize all syntax in the language.
    Once the lexer is complete, syntax analysis will be possible to validate
    and ensure that the provided script file is one which matches the language
    specification.

- [ ] Supporting C++ Libraries

    The supporting C++ libraries provide the required functionality to
    operate POSY on the C++ side.

- [ ] Syntax Parsing

    Necessary for ensuring proper syntax structure and language implementation.
    This is the meat and potatoes.

