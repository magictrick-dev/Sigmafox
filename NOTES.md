So far the parser works, but fails to properly handle the symbols table.

    -   Identifier declared, set as undefined on declaration statements.
    -   Assignment expression statements then evaluate the type.
    -   Undefined types can be used anywhere except on RHS expression statements.

The lexer will need to be adjusted so that the tokenizer feeds procedurally. This
will reduce dependency on the tokens list.

    -   Lexer now takes the source file buffer as well as its offset and step.
    -   Parser directly calls the lexer API to get tokens.
    -   Tokens will no longer persist as long-term values and should be converted
        to literals where they are needed.

String pool needs to be set up in order to store identifier names. The string pool
shouldn't be a searchable set, only to store strings with pointers that reference
their initial location.

    -   A hash table can be used to properly discover if a string has been added
        to the pool. This will reduce the overall size of the pool since identifier
        names will most likely be repeated several times.
    -   The string pool API is used by the symbol table, and therefore connected
        in use-case. Additionally, strings that compose comments and string buffers
        are also stored here.

The modifications above require dynamic resizing and will not fit with single-stack
based approach. The modifications require some changes to the arena allocator such
that we have a region for AST nodes and another for hash tables and string pools.

    -   We make the string pool fixed size. A compiler flag can adjust this to
        be any larger size. If the string pool size is reached, we force-close
        and display a warning that the compilation can not be complete without
        increasing the memory pool size capacity.

Finally, we should probably make the final push to use C entirely. Therefore,
we will loose some of the handy-dandy features of C++. This requires some typedefs.

```C
#include <stdint.h>

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef float       r32;
typedef double      r64;

typedef int32_t     b32;
typedef int64_t     b64;
```

We are now reaching the point where we need CLI flags then.

    -   CLI parser.
    -   Double dashes have parameters.
    -   Single dashes are toggle-flags.
    -   File names always come first, then any combination of parameters/flags.
    -   First pass joins flags.
    -   Second pass validates grammar.
    -   Third pass validates file paths.
    -   Output is a structure which contains the list of source files and options.

    -   A set of flags for:

            --output-name [string]
                Example: sigmafox fibonacci.fox --output-name fibonacci
                Default: --output-name main

                Sets the output name of either compiled binary or the entry point.
                File extensions are automatically set.

            --output-directory [directory]
                Example: sigmafox fibonacci.fox --output-directory ./build
                Default: --output-directory ./

                Sets the directory where all build files are stored.

            -c, --compile
                Example: sigmafox fibonacci.fox -c -o build/fibonacci.exe
                Default:

                Enables direct-to-binary compilation.

            -t, --trim-comments
                Example: sigmafox fibonacci.fox -t
                Default:

                Removes all comments from the generated C++ file.

            --memory-limit-size [memory-size]
                Example: --memory-limit 4GB
                Default: --memory-limit 4GB

                Sets the amount of memory the transpiler reserves at startup.
                The transpiler will error out with an appropriate message if
                this capacity is reached.

            --string-pool-limit [memory-size]
                Example: --string-pool-limit 16MB
                Default: --string-pool-limit 16MB

                Sets the amount of memory reserved within the string pool. The
                transpiler uses this to store identifier names and string buffers
                during the parsing phase. The transpiler will error out with an
                appropriate message if this capacity is reached. The size of the
                string pool proportionally reduces the amount of free-space the
                transpiler has for other allocations.

```C
typedef struct command_line_interface
{
    b32         trim_comments;
    b32         direct_compile;
    u64         memory_limit;
    u64         string_pool_limit;
    const char *output_name;
    const char *output_directory;
} command_line_interface;

b32     command_line_parse(command_line_interface *cli, int argument_count, char **argument_list);
```

As far as the lexer goes, the new interface will probably end up like this:

```C
typedef struct source_token
{
    const char *source;
    u64         offset;
    u64         length;
    u32         type;
} source_token;

typedef struct source_file_tokenizer
{
    const char *source_file_name;
    const char *source_buffer;
    u64         step;
    u64         offset;
} source_file_tokenizer;

void get_token_from_tokenizer(source_file_tokenizer *tokenizer, source_token *token);
```

Essentially, the tokenizer will no longer tokenize the full source file. Rather, it
is queried as the parser fetches tokens. The parser is designed to inspect the "previous",
"current", and "next" tokens respectively, meaning that we don't have to hold a full list
of tokens at once. The parser can simply use the above API to grab the tokens as it needs
them.

This opens up some additional room for optimizations that otherwise weren't possible. We
can perform overlapped file reads during parsing by scheduling a buffer queue. Two buffers
are pre-loaded. When one buffer is fully consumed by the tokenizer, it launches a helper
thread to write to the just-consumed buffer while it starts consuming the next. This will
help with larger text files over some arbitrary size. This means we don't need to fully
occupy our memory with the entire source file, only bits at a time.

```
source/compiler/tokenizer.h
source/compiler/tokenizer.cpp
source/compiler/environment.h
source/compiler/environment.cpp
source/compiler/parser.h
source/compiler/parser.cpp
source/compiler/generator.h
source/compiler/generator.cpp
```

There is another architectural problem that hasn't been address: multi-file includes.
The parser itself can be ran for each of the provided files at transpile time.

This means that there are symbols that are imported from an "include" are unresolved.
We could do a symbol resolution pass which basically takes the file with the "entry point"
and subsequently merge symbol tables at import.

Essentially, we parse, allow unresolved bindings, then merge.

    -   First pass basically encounters an import statement and appropriate updates the
        AST that there is a import. Call it an import list.
    -   This import list is a job queue. For now, we make this job queue single-threaded.
        Once the parser completes one in the list, it begins the next.
    -   All imports are resolved for circular dependencies. A call-graph needs to be made
        ensure that we don't have circular imports. As long as a file doesn't attempt to
        include a file that attempts to include itself at any point, there is no issue.
    -   If the import is resolved, it is not already parsed, then it is parsed.
    -   The job is consumed and the next job is started. This process is repeated until
        no more imports are in the import list.

Symbol resolution then happens at a second pass of the transpiler. It will then cyle down
the AST, resolving all symbols in chronological order. Any unresolved symbols are then
given as an error to the user.

The third pass of the transpiler is the code generation pass. We could, in all reality,
perform this step at the same time we resolve symbols, though the symbol resolution
pass could, in theory, skip certain AST nodes. Either way, this is the final pass of
the transpiler where the AST is fully converted to C++.