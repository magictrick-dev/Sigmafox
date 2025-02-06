# Sigmafox COSY Compiler

The Sigmafox COSY compiler is a compiler for the COSY scripting language. The primary
function of Sigmafox is to translate COSY scripts to executable C++ projects which
can be then compiled into binaries to run beam physics simulations. Such an architecture
allows for performance optimizations on the C++ level that would ordinarily not be
available with the traditional FORTRAN 77 COSY compiler that is currently in use.

# Environment Setup

In order to build and use the Sigmafox COSY compiler, you must first have a suitable
C++ compiler for your system. This project uses cmake to create the necessary build
files, so use your platform's preferred C++ development environment tools. For Windows,
this requires Microsoft Visual Studio. For most Linux distributions, they come native
with the gcc compiler, but may not have cmake installed.

To generate the build files:

```
cmake -B build
```

After generating the build files, you can compile the project with:

```
cmake --build ./build
```

If everything goes well, you should find the compiler in a folder called `bin/`
which you can then add to your environment paths for your respective platform.

# Language Documentation

The language specification closely follows the original COSY language specification
as outlined in the manual from [BMT Dynamics](https://www.bmtdynamics.org/cosy/) at
Michigan State University. The manual can be found [here](https://www.bmtdynamics.org/cosy/manual/COSYProgMan102.pdf).
Some changes and additions have been made to the language specification. In most cases, 
these changes are non-breaking and extend the functionality of the language. Additional
changes are opt-in through compiler flags. Despite the effort to ensure that some
of the changes are non-breaking to existing code basis, there is a very real possibility
that older scripts may not directly compile correctly. Please look over your existing
code to ensure that it conforms with the specification as outlined below.

The Sigmafox variant of the COSY scripting language allows for keywords to be upper
or lowercase. This is not true for non-keyword identifiers which are case-sensitive
and must begin with a alphabetical character. Identifiers may contain underscores.

## The COSY Project Layout

To design a project, create a file `.fox` file. The file that you are compiling
is labeled as your *entry point* file and must contain a `begin` and `end` block.
The global scope of this, more specifically all code written *outside* of the `begin`
and `end` blocks must either be functions, procedures, or an `include` directives.
This is a minor depature from the original specification to support multiple include
files.

```
include "utils/math.fox";
include "utils/intrinsics.fox";
include "helpers.fox"

procedure print_statement stm;
    write 4 stm;
endprocedure;

function add_numbers a b;
    add_numbers := (a + b);
endfunction;

begin;
    print_statement "hello world";
    print_statement add_numbers(8, 16) + 2
end;
```

Unlike the original COSY specification, the Sigmafox compiler allows for multiple
include files to help break up project structure. The exact structure of your include
files are mirrored in the translated output, so it does pay to have a well-organized
project. Included file paths are resolved relative to the *entry point* file.

## Comments

All comments are block comments in COSY. To comment a block of code, surround it
with `{` and `}`. These are omitted during translation.

```
function add_numbers a b;
    { variable result 4 := a + b; }
    add_numbers := a + b;
endfunction;
```

Future work is being done to allow for some comments to make their way over to the
translated output, but it is not a high priority feature.

## Variables, Declarations, and Definitions

Variables require a size to immediately follow the declaration. This is a legacy
feature of the COSY-specification and doesn't evaluate to anything after transpilation.

```
variable foo 4;
```

The language was extended to include inline definitions. It is now legal to provide
an assignment expression immediately after declaring a variable. This feature is enabled
by default and it is recommended to use this format to ensure that variables are
initialized after translation.

```
variable foo 4 := (2 * 5) + (80 - 39);
```

Additionally, providing additional expressions after the size parameter for a variable
defines arrays. The COSY specification defines arrays to use one-based indexing; basically
arrays start at 1 instead of 0 like most traditional languages have it.

Arrays are n-dimensional and do not have an upper-bound for how large
they can be. Naturally, this means that array expressions become rather complex to work
with on the translation side. For one-dimensional and two-dimensional arrays, they transpile
to their native counter-parts, but dimensions beyond 2+ degenerate to nested arrays.

```
variable array2d 4 4 4;
array2d(1,1) := 1; { NOTE:  COSY uses one-based indexing.   }
array2d(1,2) := 1; {        This applies to all dimensions. }
array2d(1,3) := 1;
array2d(1,4) := 1;
array2d(2,1) := 2;
array2d(2,2) := 2;
array2d(2,3) := 2;
array2d(2,4) := 2;
{ and so on... }
```

## Loops

Loops are straight forward. The first parameter is the loop's iterator. The second
and third parameter are your start and stop expressions, inclusive. Finally, the optional
fourth parameter is your step expression. The `break` keyword doesn't not exist, so
you must manually short-circuit your loops.

```
loop idx 1 10 2;
    write 4 "Current value is: " idx;
endloop;
```

There is also a while loop which checks the expression after each iteration.

```
variable a 4 := 0;
variable b 4 := 10;
while a < b;
    a := a + 2;
endwhile;
```

The original specification COSY enforces that the loop's iteration variable consistently
steps after each iteration. However, the Sigmafox doesn't enforce this rule and allows
loops to short-circuit like traditional languages. You will need to program your loop
to maintain consistent steppings if this is a feature you require.

## Conditionals

Conditionals in COSY operate the same way in other languages and require an expression
to evaluate as a boolean.

```
if a < b;
    write 4 "A is less than B.";
endif;
```

You can also chain else expressions:

```
if a < b;
    write 4 "A is less than B.";
elseif a > b;
    write 4 "B is less than A.";
endif;
```

Rather non-intuitively, though, there is no keyword for a plain `else`, so you must
create a truthy statement to create an `else` condition:

```
if a < b;
    write 4 "A is less than B.";
elseif a > b;
    write 4 "B is less than A.";
elseif 1
    write 4 "A is the same as B.";
endif;
```

In the future, the else keyword may be added to make this more intuitive.

## Functions and Procedures

Functions provide a means to group and isolate a block of code for reuse in other
sections of your program. Unlike functions in other languages, returning a value
requires setting the name of the function to the value or expression you wish to
return.

```
function add_numbers a b;
    { Perform some operation. }
    variable result 4 := a + b;

    { NOTE: You must set the value of the function name.    }
    {       It is an error if you don't.                    }
    add_numbers := result; 
endfunction;
```

Invoking functions are a simple matter and can occur anywhere an expression may occur.

```
variable my_sum 4 := add_numbers(8, 16) + 2;
```

Much like functions, procedures provide a similar way of organizing your code except
it does not return any values nor can you invoke them in expressions. Instead, they
must stand alone as an expression statement.

```
procedure print_statement stm;
    write 4 stm;
endprocedure;

print_statement "hello world";
```

Functions and procedures may be defined within any block scope, including the global
scope. They follow the same rules as variables; variable shadowing produces warnings,
while redefining in the same scope produces errors. Although they behave much like
variables, they may not be reassigned to another function. Functions and procedures 
may translate to lambdas outside of the global scope.

# Documentation

The development of Sigmafox is still in its early stages and there isn't a lot of
documentation on the implementation other than what is directly available in the
comments of the source code. The compiler uses recursive descent to generate an AST
which is then traversed during the translation pass. A good bulk of the code resides
in the parser and visitor subroutines.

# License

This code is developed for the Department of Physics at Northern Illinois University.
Please reach out to them for further information.
