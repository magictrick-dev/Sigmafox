#ifndef SOURCE_CORE_CLI_H
#define SOURCE_CORE_CLI_H
#include <core/definitions.h>
#include <core/arena.h>

// --- CLI Parser Outline ------------------------------------------------------
//
// The command line parser essentially constructs and validate CLI arguments.
// The CLI parser isn't exactly meant to be a strict parser, it essentially
// lazy-evaluates strings into certain types and ensures that arguments parse
// correctly, followed by a single source file, followed by more arguments.
//
// NOTE(Chris): The code isn't pretty and could do with a refactor if a more
//              robust CLI parser is required. For now, we just want some sort
//              of way of modifying the runtime behavior through the startup
//              procedure. This happens to be a kinda okayish way to do so.
// 
// CLI Grammar
//
//      cli          : (argument)* source_file (argument)*
//      argument     : parameter | switch
//      switch       : "-"(FLAG*)
//      parameter    : "--output-name" STRING | "--output-directory" PATH | "--compile"
//                     "--trim-comments" | "--memory-limit-size" ( SIZE | NUMBER ) | 
//                     "--string-pool-limit" ( SIZE | NUMBER ) | "--help"
//
// CLI Token Types
//
//      CLI_TOKEN_PARAMETER
//      CLI_TOKEN_SWITCH
//      CLI_TOKEN_STRING
//      CLI_TOKEN_NUMBER
//      CLI_TOKEN_SIZE
//      CLI_TOKEN_PATH
//
// Parameters are parsed by name and lead with a double dash. The following tokens
//      are required to match what comes later. Some parameters are aliases for
//      switches and are handled by the parser appropriately.
//
// Switches are parsed by character and lead with a single dash. Multiple switches
//      can be toggled by appending multiple characters. Example of "-c" and "-t"
//      written as "-ct" instead.
//
// Strings are basically anything that doesn't lead with a number. A string can
//      also be described in quotes, allowing for spaces.
//
// Numbers begin with a number, integers only, and ends with a number.
//
// Sizes begin with a number, integers only, and end with a size identifier of
//      either "KB, MB, and GB". Case-insensitive. The result is converted to
//      a number, tokenized as size. KB = n-bytes * 1024, MB = n-bytes * (1024*1024),
//      and GB = n-bytes * (1024 * 1024 * 1024).
//
// Paths are strings which are properly validated through a system call. This
//      ensures that the program can properly access the path.
//
// -----------------------------------------------------------------------------
//
// -h, --help
//     Example: sigmafox --help
//     Default:
//
//     Displays the help dialogue and the list of commands. This is automatically
//     invoked in short-form when the CLI parser fails or in long-form explictly
//     when invoked by the user. If the help flag is provided, execution doesn't
//     continue and the program automatically exits after parsing.
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

typedef struct runtime_flags
{
    b8 unused_a;
    b8 unused_b;
    b8 compile;
    b8 unused_d;
    b8 unused_e;
    b8 unused_f;
    b8 unused_g;
    b8 help;
    b8 unused_i;
    b8 unused_j;
    b8 unused_k;
    b8 unused_l;
    b8 unused_m;
    b8 unused_n;
    b8 unused_o;
    b8 unused_p;
    b8 unused_q;
    b8 unused_r;
    b8 unused_s;
    b8 trim_comments;
    b8 unused_u;
    b8 unused_v;
    b8 unused_w;
    b8 unused_x;
    b8 unused_y;
    b8 unused_z;
} runtime_flags;

typedef struct runtime_parameters
{

    union
    {
        b8 flags[26];
        runtime_flags options;
    };

    u64 memory_limit;
    u64 string_pool_limit;

    cc64 output_directory;
    cc64 output_name;

    cc64 source_file_path;

    b32 helped;
    i32 arg_current;
    i32 arg_count;
    char **arguments;
} runtime_parameters;

typedef enum cli_parser_code
{
    CLI_PARSER_ERROR        = -1,
    CLI_PARSER_BREAK        =  0,
    CLI_PARSER_CONTINUE     =  1,
} cli_parser_code;

typedef enum cli_token_type
{
    CLI_TOKEN_ARGUMENT_OUTPUT_NAME,
    CLI_TOKEN_ARGUMENT_OUTPUT_DIR,
    CLI_TOKEN_ARGUMENT_HELP,
    CLI_TOKEN_ARGUMENT_COMPILE,
    CLI_TOKEN_ARGUMENT_TRIM_COMMENTS,
    CLI_TOKEN_ARGUMENT_MEM_LIMIT,
    CLI_TOKEN_ARGUMENT_POOL_LIMIT,

    CLI_TOKEN_SWITCH,

    CLI_TOKEN_STRING,
    CLI_TOKEN_NUMBER,
    CLI_TOKEN_FILE,
    CLI_TOKEN_PATH,

    CLI_TOKEN_EOA                   = 100,
    CLI_TOKEN_UNDEFINED_ARGUMENT    = 200,
    CLI_TOKEN_UNDEFINED_SWITCH      = 300,
    CLI_TOKEN_UNDEFINED             = 999, 
} cli_token_type;

typedef struct cli_token
{
    cli_token_type type;
    i32 index;
    u64 value;
} cli_token;

void cli_parser_display_help_long();
void cli_parser_display_help_short();
void cli_parser_get_next_token(runtime_parameters *parameters, cli_token *token);
b32 cli_parser_match_argument(runtime_parameters *parameters);
b32 cli_parser_match_default(runtime_parameters *parameters);
b32 cli_parser_match_string_caseless(cc64 string, cc64 match_to);
b32 command_line_parse(runtime_parameters *parameters);

#endif
