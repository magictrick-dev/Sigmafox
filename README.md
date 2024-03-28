# Sigmafox

A transpiler for COSY/FOXY scripting language.

## Building Sigmafox

In order to build the project from source, you will need [cmake](https://cmake.org/)
installed and accessible from the commandline. You can make use of the build
scripts or you can simply run `cmake --build ./build` and `cmake -B ./build` to
configure and compile the project.

## Development Notes

- Lexer

    When tokens are generated, it is sometimes helpful to know the line offset
    and line number of their original location. The line offset is the relative
    byte-offset within the source text and the line number is the literal line
    (exclusive of zero) from the top of the source file. The line offset can be
    subtracted from the token offset to get the column position of the token within
    the source file. So far, these values are calculated as-needed, but it isn't
    necessary since the lexer itself can track the line offset and line number as
    it parses the source. These values can be fed into the token constructor such
    that they can be cached rather than calculated at a later time, saving some
    computation time.

- Threading the Lexer

    Multiple text files can be lexed at the same time using threading. This is
    especially helpful if source files become significantly large. Since the lexer
    doesn't perform any additional dependency processing, we can implement this
    fairly early on with zero overhead.

- Multifile Projects

    While not in the COSY/FOXY language specification, this can be added through
    some modifications with how files are included. Since the include keyword is
    already defined (for single-file imports), we can create a import heirarchy
    which gives a top-down view of the order of which to place symbols in the symbols
    table. That way, multifile projects are possible.
