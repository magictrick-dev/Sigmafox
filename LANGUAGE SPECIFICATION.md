# Sigmafox / COSY Language Specification

This document describes the COSY syntax and the proposed modifications
for the Sigmafox implementation of COSY. It's important that the original
COSY syntax is preserved in such a way that little modification of existing
script files is needed.

From the COSY manual, we have an outline of the complete COSY syntax table.
This table doesn't encompass all symbols within the language, but contains
a list of keywords that need to be implemented.

### BEGIN/END

Begin and end statements in the *original* COSY syntax demark where a
program begins and ends. COSY defines this as the "main" program segment.
This main program segment *must* be present within the file. The `BEGIN`
keyword should be the first processed keyword, and the `END` keyword should
be the last.

**Proposed Changes:**

We want to modularize the code, so we want to take the `begin` and `end`
statements and convert them to functions. Therefore, we can treat the
begin and end as legacy keywords for `function main` and `endfunction`.
This will drop support for the begin/end keywords that only serve as a
marker for a block of executable code. We allow the function called main
take over as the primary entry point. (During transpilation, this will
happen regardless of whether or not we honor this change.)

### VARIABLE \<name\> \<memory size\> { \<array dimension\> ... }

Variables, of course, define variables in COSY. COSY defines names to have a
fixed length of 32 characters, case-insensitive, and may contain underscores
and numbers after the first alpha-character.

Variables are considered "polymorphic" in COSY, but in more general terms,
it means that variables are loosely typed. At initialization time, all variables
are considered "reals". Like most
loosely typed languages, these variables can assume the type which composes
them at run-time, as needed, so long as it fits the *memory size* that initially
defined the variable.

When variables are created, a memory size is pre-defined. This memory size
determines how much of *whatever* can be put into the variable.

You can also optionally define the variable to be an array. For each dimension
you add, you get: `memory-size * dimension-a * dimension-b * ... * dimension-n `
as the total allocation size. This behavior is standard save for the fact that
the actual size of the allocation is determined by the dimension of the array
and the size that each element of the array is.

Variables in COSY obey the top-down rule (variables don't leak upwards),
but variables of the same name overwrite previous variables of that name.
Which means it is not a syntax error for a variable to be redefined with
the same name later on in the scope of the program.

**Proposed Changes:**

We leave variables as they are as a legacy component and support them through
a custom datatype in C++ so that there aren't any changes to how the program
works. What we can do change is the system that honors the memory-size. Instead,
this becomes a suggestion to the transpiler about how much memory it could
*potentially* consume, not how much is physically reserved, and then let the C++
code handle memory management as required by the user. This reduces overhead
dramatically by ensuring

### FUNCTION/PROCEDURE \<name\> \<arguments ...\>

COSY defines functions as have some sort of mathematical return value,
and procedures as not. They both internally run some operations.

Invocations of functions can be used in expressions, procedures can
not. Largely, this might be a product of being able to differentiate whether or
not something actually returns a value or not, but ultimately becomes moot 
when the invocation itself tells you nothing about whether or not something
returns something for an expression.

**Proposed Changes:**

Merge these two, C++ sees no difference except in the return types. During
transpilation, our only check if it is a valid expression is if it was marked as
a function over a procedure. This makes it a little easier to check.

### \<name\> := \<expression\>

This is COSY's assignment expression. There are heirarchies that COSY defines
that are evaluated for expressions, to which can be overrided with appropriate
parenthesis. 

Operator Precedence:

* `<`: 2 (Lesser)
* `>`: 2 (Greater)
* `=`: 2 (Equals to)
* `#`: 2 (Not equals to)
* `&`: 2 (Concatenation)
* `+`: 3 (Addition)
* `-`: 3 (Subtraction)
* `*`: 4 (Multiplication)
* `/`: 4 (Division)
* `^`: 5 (Exponentiation) (Both power func. intrinsic & symbolic)
* `|`: 6 (Extraction)
* `%`: 7 (Derivation)
* `%(-1)` : 7 (Integration)

### Remaining Keywords

```
IF <expression> ;
ELSEIF <expression> ; 
ENDIF ;

WHILE <expression> ; 
ENDWHILE ;

LOOP <name> <beg> <end> ; 
ENDLOOP ;

PLOOP <name> <beg> <end> ; 
ENDPLOOP <comm. rules> ;

FIT <variables> ; 
ENDFIT <parameters, objectives> ;

WRITE <unit> <expressions> ; 

READ <unit> <names> ;

SAVE <filename> ; 

INCLUDE <filename> ;
```


