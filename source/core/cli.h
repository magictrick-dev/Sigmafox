#ifndef SOURCE_CORE_CLI_H
#define SOURCE_CORE_CLI_H
#include <core/definitions.h>

// --- CLI Parser Outline ------------------------------------------------------
//
// Another parser written in a parser. We only need to create the scanner and
// the appropriate in-line parser.
// 
// --output-name [string]
//     Example: sigmafox fibonacci.fox --output-name fibonacci
//     Default: --output-name main
//
//     Sets the output name of either compiled binary or the entry point.
//     File extensions are automatically set.
//
// --output-directory [directory]
//     Example: sigmafox fibonacci.fox --output-directory ./build
//     Default: --output-directory ./
//
//     Sets the directory where all build files are stored.
//
// -c, --compile
//     Example: sigmafox fibonacci.fox -c -o build/fibonacci.exe
//     Default:
//
//     Enables direct-to-binary compilation.
//
// -t, --trim-comments
//     Example: sigmafox fibonacci.fox -t
//     Default:
//
//     Removes all comments from the generated C++ file.
//
// --memory-limit-size [memory-size]
//     Example: --memory-limit 4GB
//     Default: --memory-limit 4GB
//
//     Sets the amount of memory the transpiler reserves at startup.
//     The transpiler will error out with an appropriate message if
//     this capacity is reached.
//
// --string-pool-limit [memory-size]
//     Example: --string-pool-limit 16MB
//     Default: --string-pool-limit 16MB
//
//     Sets the amount of memory reserved within the string pool. The
//     transpiler uses this to store identifier names and string buffers
//     during the parsing phase. The transpiler will error out with an
//     appropriate message if this capacity is reached. The size of the
//     string pool proportionally reduces the amount of free-space the
//     transpiler has for other allocations.

typedef union runtime_flags
{
    b8 flags[52];
    struct
    {
        flag_
    };
};

typedef struct runtime_parameters
{
    u64 memory_limit;
    u64 string_pool_limit;
} runtime_parameters;

#endif
