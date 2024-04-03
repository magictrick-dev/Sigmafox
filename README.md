# Sigmafox

A transpiler for COSY/FOXY scripting language.

## Building Sigmafox

In order to build the project from source, you will need [cmake](https://cmake.org/)
installed and accessible from the commandline. You can make use of the build
scripts or you can simply run `cmake --build ./build` and `cmake -B ./build` to
configure and compile the project.

## Development Notes

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

- Define Language Specification Documentation

- Parser Completion

    Convert the lex'd source file to a parsed AST which uses the language
    specification to properly define whether the syntax is in the language.

## Proposed Language Specification Changes

Below are the proposed language specification changes to make multifile includes
more intuitive. These changes should be backwards compatible.

### Named Begin/End Blocks

The language specification will maintain full compatibility to legacy COSY script syntax
with some modifications to how scope is handled. First, the `begin` and `end` blocks
now define what is known as "module scope". An unnamed `begin` is implicitly the
main module scope. There can only be one main module scope. This will maintain
full backwards compatibility with legacy COSY script while also allowing for named
`begin` blocks.

The below script shows a valid COSY script using implicit main. This setup is still
valid in the updated language specification.

```
begin;

    procedure print_hello;

        { Start a loop at zero. }
        variable loop_count 1;
        loop_count := 0

        { Print ten times. }
        while loop_count < 10;
            write 6 "Hello world."
            loop_count := loop_count + 1
        endwhile;

    endprocedure;

    print_hello;

end;
```

Here is the same script, except it uses the new module scope definition for begin/end.


```

{ This procedure is now a global procedure and visible to everything below it. }
procedure print_hello;

    { Start a loop at zero. }
    variable loop_count 1;
    loop_count := 0

    { Print ten times. }
    while loop_count < 10;
        write 6 "Hello world."
        loop_count := loop_count + 1
    endwhile;
    
endprocedure

{ You can also write 'begin main;' to be explicit this is main. }
begin;
    
    print_hello;

end;
```

### Modules

Since begin and end blocks can be named, and more than one file can be included,
it makes sense that these included files may also be modularized so as to maintain
both stateful behavior and containerize code local to its intended functionality.

Like above, except now we can a hello module in a file called "hello.foxy".

```
begin hello;
    
    procedure print_hello;

        { Start a loop at zero. }
        variable loop_count 1;
        loop_count := 0

        { Print ten times. }
        while loop_count < 10;
            write 6 "Hello world."
            loop_count := loop_count + 1
        endwhile;
        
    endprocedure

end;
```

If you include this file to another file, called "main.foxy":

```
include "hello.foxy"

begin main;

    hello.print_hello;

    { This would be an error since print_hello is contained inside the hello mdoule. }
    { print_hello; }

end;
```

You can see that the exported "hello" module comes from "hello.foxy" as is. This
may not be very clear without looking into the file, so it would be useful to rename
the exported module:

```
include "hello.foxy" as hello_module;

begin main;

    hello_module.print_hello;

end;
```

### Shorthand Operators

Potential to add shorthand operators like `:+`, `:-`, `:*`, `:/` to make expressions
like `loop_count := loop_count + 1` shorter with `loop_count :+ 1`.

Also, maybe increment and decrement shorthands, `:++`, `:--` to make this expression
even short: `loop_count:++`.

