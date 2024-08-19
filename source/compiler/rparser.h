#ifndef SOURCE_COMPILER_RPARSER_H
#define SOURCE_COMPILER_RPARSER_H
#include <core/definitions.h>
#include <core/arena.h>
#include <compiler/stringpool.h>
#include <compiler/symboltable.h>

#define STRING_POOL_DEFAULT_SIZE    SF_MEGABYTES(256)
#define SYMBOL_TABLE_DEFAULT_SIZE   SF_MEGABYTES(256)

// --- Tokenizer ---------------------------------------------------------------
//
// The source tokenizer takes a given raw-text source file and attempts to match
// tokens that conform to the language specification. It works by examining each
// character and selectively determining if it conforms to a certain pattern.
// Think regex, except faster.
//
// The primary interface to concern yourself with is the source_tokenizer structure
// and the source_tokenizer_get_next_token(...) function. This will feed back a token,
// error or not. The main idea is that certain tokenizer errors can be tokenized
// themselves, allowing for rather accurate error reporting.
//

typedef struct source_tokenizer
{
    cc64 file_path;
    c64 source;
    u64 offset;
    u64 step;
} source_tokenizer;

typedef enum source_token_type
{
    TOKEN_COMMENT_BLOCK,
    TOKEN_LEFT_PARENTHESIS,
    TOKEN_RIGHT_PARENTHESIS,
    TOKEN_SEMICOLON,
    TOKEN_COLON_EQUALS,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_FORWARD_SLASH,
    TOKEN_CARROT,
    TOKEN_EQUALS,
    TOKEN_LESS_THAN,
    TOKEN_LESS_THAN_EQUALS,
    TOKEN_GREATER_THAN,
    TOKEN_GREATER_THAN_EQUALS,
    TOKEN_HASH,
    TOKEN_AMPERSAND,
    TOKEN_PIPE,
    TOKEN_PERCENT,

    TOKEN_INTEGER,
    TOKEN_REAL,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,

    TOKEN_KEYWORD_BEGIN,
    TOKEN_KEYWORD_ELSEIF,
    TOKEN_KEYWORD_END,
    TOKEN_KEYWORD_ENDFIT,
    TOKEN_KEYWORD_ENDIF,
    TOKEN_KEYWORD_ENDFUNCTION,
    TOKEN_KEYWORD_ENDLOOP,
    TOKEN_KEYWORD_ENDPLOOP,
    TOKEN_KEYWORD_ENDPROCEDURE,
    TOKEN_KEYWORD_ENDSCOPE,
    TOKEN_KEYWORD_ENDWHILE,
    TOKEN_KEYWORD_FIT,
    TOKEN_KEYWORD_FUNCTION,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_INCLUDE,
    TOKEN_KEYWORD_LOOP,
    TOKEN_KEYWORD_PLOOP,
    TOKEN_KEYWORD_PROCEDURE,
    TOKEN_KEYWORD_READ,
    TOKEN_KEYWORD_SAVE,
    TOKEN_KEYWORD_SCOPE,
    TOKEN_KEYWORD_VARIABLE,
    TOKEN_KEYWORD_WHILE,
    TOKEN_KEYWORD_WRITE,
    TOKEN_NEW_LINE,

    TOKEN_EOF = 1000,
    TOKEN_UNDEFINED,
    TOKEN_UNDEFINED_EOF,
    TOKEN_UNDEFINED_EOL,
} source_token_type;

typedef struct source_token
{
    source_token_type type;
    c64 source;
    u64 offset;
    u64 length;
} source_token;

cc64    source_token_string_nullify(source_token *token, char *hold_character);
void    source_token_string_unnullify(source_token *token, char hold_character);
void    source_token_position(source_token *token, i32 *line, i32 *col);

cc64    source_tokenizer_token_type_to_string(source_token *token);
b32     source_tokenizer_eof(source_tokenizer *tokenizer);
b32     source_tokenizer_eol(source_tokenizer *tokenizer);
b32     source_tokenizer_isalpha(source_tokenizer *tokenizer);
b32     source_tokenizer_isnum(source_tokenizer *tokenizer);
b32     source_tokenizer_isalnum(source_tokenizer *tokenizer);
b32     source_tokenizer_match(source_tokenizer *tokenizer, u32 count, ...);
char    source_tokenizer_peek(source_tokenizer *tokenizer, u64 offset);
char    source_tokenizer_consume(source_tokenizer *tokenizer, u64 count);
void    source_tokenizer_set_token(source_tokenizer *tokenizer, source_token *token, source_token_type type);
void    source_tokenizer_check_identifier(source_tokenizer *tokenizer, source_token *token);
void    source_tokenizer_synchronize(source_tokenizer *tokenizer);
b32     source_tokenizer_consume_whitespace(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_comments(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_newline(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_symbols(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_numbers(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_strings(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_identifiers(source_tokenizer *tokenizer, source_token *token);
void    source_tokenizer_get_next_token(source_tokenizer *tokenizer, source_token *token);
void    source_tokenizer_initialize(source_tokenizer *tokenizer, c64 source, cc64 path);

// --- Parser ------------------------------------------------------------------
//
// The parser generates a traversable AST by generating a contiguous list of
// syntax nodes. The parser uses a mixture of recursive descent parsing for
// expressions and Pratt parsing for statements.
//
// When a parser completes, the head node is returned. You can expect the entire
// AST nodes list to be contiguous, in parse order. Additionally, the parser also
// inspects for an "entry" node, which identifies complete program conformance.
//
// If parsing fails with an error, the source parser *always* returns NULL. There
// is no additional work that can be done on a program which contains errors.
//
// Language Grammar
//
//      program                 :   (global_statement)* "begin" (statement)* "end" ; EOF
//      global_statement        :   import_statement | procedure_statement | function_statement
//      statement               :   declaration_statement | expression_statement | invoke_statement
//                                  scope_statement | while_statement | loop_statement
//                                  conditional_statement | procedure_statement | function_statement
//      import_statement        :   "import" identifier ;
//      function_statement      :   "function" identifier (identifier)* ; (statement)* "endfunction" ;
//      procedure_statement     :   "procedure" identifer (identifier)* ; (statement)* "endprocedure" ;
//      invoke_statement        :   identifier (expression)* ;
//      declaration_statement   :   "variable" identifier expression
//                                  (expression)* (definition_statement)? ;
//      definition_statement    :   ":=" expression
//      expression_statement    :   expression ;
//      scope_statement         :   "scope" (statement)* "endscope" ;
//      while_statement         :   "while" epxression ; (statement)* "endwhile" ;
//      loop_statement          :   "loop" identifier expression expression (expression)? ;
//                                  (statement)* "endloop" ;
//      if_statement            :   "if" expression ; (statement)* (elseif_statement | if_terminate)
//      elseif_statement        :   "elseif" expression ; (statement*) (elseif_statement | if_terminate)
//      if_terminate            :   "endif" ;
//      expression              :   assignment
//      assignment              :   identifier ":=" assignment | equality
//      equality                :   comparison (("=" | "#") comparison)*
//      comparison              :   term (("<" | "<=" | ">" | ">=") term)*
//      term                    :   factor (("+" | "-") factor)*
//      factor                  :   unary (("*" | "/") unary)*
//      unary                   :   ("-" unary) | call
//      call                    :   primary ( "(" arguments? ")" )?
//      primary                 :   NUMBER | STRING | identifier | "(" expression ")"
//
// Grammar Notes
//      
//      Comment statements and new line statements are elevated to grammatical
//      constructs since we want to preserve their meaning during transpilation.
//      Therefore, we account for this by generating actual syntax nodes for them.
//
//      In the original COSYScript syntax, procedures and functions *must* be
//      defined within the "begin" and "end" tags, but we remove that restriction
//      for better code organization. The language allows for nested functions,
//      but transpiled output may not.
//
//      COSY doesn't allow for recursion, but we will allow it. This will need
//      to be checked in a separate pass. We will essentially need to walk the
//      tree and generate a call stack so that recursion is properly discovered.
//
//      Function calls may exist in expressions, but procedures may not. Therefore,
//      invoke statements exist to catch this difference. The syntax differences
//      make this tricky to install in expressions, but procedure calls don't work
//      in expressions anyway, so we elevate them up to statement status.
//      

typedef struct syntax_node          syntax_node;
typedef struct binary_syntax_node   binary_syntax_node;
typedef struct unary_syntax_node    unary_syntax_node;
typedef struct grouping_syntax_node grouping_syntax_node;
typedef struct primary_syntax_node  primary_syntax_node;
typedef struct call_syntax_node     call_syntax_node;

typedef enum syntax_node_type
{
    NULL_EXPRESSION_NODE,
    BINARY_EXPRESSION_NODE,
    UNARY_EXPRESSION_NODE,
    PRIMARY_EXPRESSION_NODE,
    CALL_EXPRESSION_NODE,
    GROUPING_EXPRESSION_NODE,
    ASSIGNMENT_EXPRESSION_NODE,
    PROCEDURE_CALL_EXPRESSION_NODE,
    FUNCTION_CALL_EXPRESSION_NODE,
    EXPRESSION_STATEMENT_NODE,
    COMMENT_STATEMENT_NODE,
    NEWLINE_STATEMENT_NODE,
    DECLARATION_STATEMENT_NODE,
    SCOPE_STATEMENT_NODE,
    LOOP_STATEMENT_NODE,
    WHILE_STATEMENT_NODE,
    IF_STATEMENT_NODE,
    ELSEIF_STATEMENT_NODE,
    PROCEDURE_STATEMENT_NODE,
    FUNCTION_STATEMENT_NODE,
    PARAMETER_STATEMENT_NODE,
    VARIABLE_STATEMENT_NODE,
    PROGRAM_ROOT_NODE,
} syntax_node_type;

typedef enum syntax_operation_type
{
    OPERATION_ADDITION,
    OPERATION_SUBTRACTION,
    OPERATION_MULTIPLICATION,
    OPERATION_DIVISION,
    OPERATION_NEGATIVE_ASSOCIATE,
    OPERATION_EQUALS,
    OPERATION_NOT_EQUALS,
    OPERATION_LESS_THAN,
    OPERATION_LESS_THAN_EQUALS,
    OPERATION_GREATER_THAN,
    OPERATION_GREATER_THAN_EQUALS,
    OPERATION_ASSIGNMENT,
} syntax_operation_type;

typedef union object_literal
{
    u64 unsigned_integer;
    i64 signed_integer;
    b64 boolean;
    r64 real;

    union {
        cc64 string;
        cc64 identifier;
    };

} object_literal;

typedef enum object_type
{
    OBJECT_UNSIGNED_INTEGER,
    OBJECT_SIGNED_INTEGER,
    OBJECT_BOOLEAN,
    OBJECT_REAL,
    OBJECT_STRING,
    OBJECT_PROCEDURE,
    OBJECT_FUNCTION,
    OBJECT_IDENTIFIER,
} object_type;

typedef struct binary_syntax_node
{
    syntax_node *left;
    syntax_node *right;
    syntax_operation_type type;
} binary_syntax_node;

typedef struct unary_syntax_node
{
    syntax_node *right;
    syntax_operation_type type;
} unary_syntax_node;

typedef struct grouping_syntax_node
{
    syntax_node *grouping;
} grouping_syntax_node;

typedef struct primary_syntax_node
{
    object_literal literal;
    object_type type;
} primary_syntax_node;

typedef struct assignment_syntax_node
{
    cc64 identifier;
    syntax_node *right;
} assignment_syntax_node;

typedef struct procedure_call_syntax_node
{
    cc64 identifier;
    syntax_node *parameters;
} procedure_call_syntax_node;

typedef struct function_call_syntax_node 
{
    cc64 identifier;
    syntax_node *parameters;
} function_call_syntax_node;

typedef struct variable_syntax_node
{
    syntax_node *size;
    syntax_node *dimensions;
    syntax_node *assignment;
    cc64 name;
} variable_syntax_node;

typedef struct scope_syntax_node
{
    syntax_node *body_statements;
} scope_syntax_node;

typedef struct while_syntax_node
{
    syntax_node *evaluation_expression;
    syntax_node *body_statements;
} while_syntax_node;

typedef struct loop_syntax_node
{
    cc64 iterator_identifier;
    syntax_node *initial_value_expression;
    syntax_node *terminate_value_expression;
    syntax_node *step_value_expression;
    syntax_node *body_statements;
} loop_syntax_node;

typedef struct elseif_syntax_node
{
    syntax_node *evaluation_expression;
    syntax_node *body_statements;
    syntax_node *else_statement;
} elseif_syntax_node;

typedef struct if_syntax_node
{
    syntax_node *evaluation_expression;
    syntax_node *body_statements;
    syntax_node *else_statement;
} if_syntax_node;

typedef struct procedure_syntax_node
{
    cc64 name;
    syntax_node *body_statements;
    syntax_node *parameters;
} procedure_syntax_node;

typedef struct function_syntax_node
{
    cc64 name;
    syntax_node *body_statements;
    syntax_node *parameters;
} function_syntax_node;

typedef struct parameter_syntax_node
{
    cc64 name;
    syntax_node *next_parameter;
} parameter_syntax_node;

typedef struct program_syntax_node
{
    syntax_node *global_statements;
    syntax_node *body_statements;
} program_syntax_node;

typedef struct syntax_node
{

    syntax_node_type type;
    syntax_node *next_node;

    union
    {
        binary_syntax_node      binary;
        unary_syntax_node       unary;
        primary_syntax_node     primary;
        grouping_syntax_node    grouping;
        assignment_syntax_node  assignment;
        variable_syntax_node    variable;
        scope_syntax_node       scope;
        while_syntax_node       while_loop;
        loop_syntax_node        for_loop;
        if_syntax_node          if_conditional;
        elseif_syntax_node      elseif_conditional;
        procedure_syntax_node   procedure;
        function_syntax_node    function;
        parameter_syntax_node   parameter;
        program_syntax_node     program;
        procedure_call_syntax_node  proc_call;
        function_call_syntax_node   func_call;
    };

} syntax_node;

typedef struct source_parser
{
    source_tokenizer tokenizer;
    source_token tokens[3];
    source_token *previous_token;
    source_token *current_token;
    source_token *next_token;

    memory_arena *arena;
    memory_arena syntax_tree_arena;
    memory_arena transient_arena;

    string_pool spool;
    symbol_table *symbol_table;

    syntax_node *entry;
    syntax_node *nodes;
    u64 error_count;
} source_parser;

// --- Expressions -------------------------------------------------------------

syntax_node* source_parser_match_primary(source_parser *parser);
syntax_node* source_parser_match_function_call(source_parser *parser);
syntax_node* source_parser_match_unary(source_parser *parser);
syntax_node* source_parser_match_factor(source_parser *parser);
syntax_node* source_parser_match_term(source_parser *parser);
syntax_node* source_parser_match_comparison(source_parser *parser);
syntax_node* source_parser_match_equality(source_parser *parser);
syntax_node* source_parser_match_expression(source_parser *parser);

// --- Expression Statements ---------------------------------------------------

syntax_node* source_parser_match_assignment(source_parser *parser);
syntax_node* source_parser_match_procedure_call(source_parser *parser);

// --- Root Statements ---------------------------------------------------------

syntax_node* source_parser_match_variable_statement(source_parser *parser);
syntax_node* source_parser_match_scope_statement(source_parser *parser);
syntax_node* source_parser_match_while_statement(source_parser *parser);
syntax_node* source_parser_match_loop_statement(source_parser *parser);
syntax_node* source_parser_match_procedure_statement(source_parser *parser);
syntax_node* source_parser_match_function_statement(source_parser *parser);
syntax_node* source_parser_match_statement(source_parser *parser);
syntax_node* source_parser_match_program(source_parser *parser);

syntax_node* source_parser_create_ast(source_parser *parser, cc64 path, memory_arena *arena);

// --- Helpers -----------------------------------------------------------------

source_token source_parser_get_previous_token(source_parser *parser);
source_token source_parser_get_current_token(source_parser *parser);
source_token source_parser_get_next_token(source_parser *parser);
source_token source_parser_consume_token(source_parser *parser);

syntax_node* source_parser_push_node(source_parser *parser);

cc64 source_parser_insert_into_string_pool(source_parser *parser, cc64 string);

b32 source_parser_should_break_on_eof(source_parser *parser);
b32 source_parser_expect_token(source_parser *parser, source_token_type type);
b32 source_parser_next_token_is(source_parser *parser, source_token_type type);
b32 source_parser_match_token(source_parser *parser, u32 count, ...);
b32 source_parser_should_propagate_error(void *check, source_parser *parser, u64 state);
b32 source_parser_synchronize_to(source_parser *parser, source_token_type type);

syntax_operation_type source_parser_convert_token_to_operation(source_token_type type);
object_type source_parser_token_to_literal(source_parser *parser, source_token *token, object_literal *object);

// --- Symbol Table Helpers ----------------------------------------------------

void source_parser_push_symbol_table(source_parser *parser);
void source_parser_pop_symbol_table(source_parser *parser);
symbol* source_parser_insert_into_symbol_table(source_parser *parser, cc64 identifier);
symbol* source_parser_locate_symbol(source_parser *parser, cc64 identifer);
b32 source_parser_identifier_is_declared(source_parser *parser, cc64 identifier);
b32 source_parser_identifier_is_declared_in_scope(source_parser *parser, cc64 identifier);
b32 source_parser_identifier_is_declared_above_scope(source_parser *parser, cc64 identifier);
b32 source_parser_identifier_is_defined(source_parser *parser, cc64 identifier);

// --- Error Handling ----------------------------------------------------------
//
// In the event that there is an error, the error handler is designed to properly
// synronize the parser and display helpful error messages as they are processed.
// Most errors are recoverable such that the parser can continue to process additional
// error messages for the user.
//
//

typedef enum parse_error_type
{
    PARSE_ERROR_HANDLED,
    PARSE_ERROR_UNDEFINED_EXPRESSION_TOKEN,
    PARSE_ERROR_EXPECTED_RIGHT_PARENTHESIS,
    PARSE_ERROR_UNEXPECTED_EOL,
    PARSE_ERROR_UNEXPECTED_EOF,
    PARSE_ERROR_UNEXPECTED_EXPRESSION_EOF_EOL,
    PARSE_ERROR_MEM_CONSTRAINT_STRING_POOL,
    PARSE_ERROR_MEM_CONSTRAINT_SYMBOL_TABLE,
    PARSE_ERROR_EXPECTED_PROGRAM_BEGIN,
    PARSE_ERROR_EXPECTED_PROGRAM_END,
    PARSE_ERROR_EXPECTED_ENDSCOPE,
    PARSE_ERROR_EXPECTED_ENDWHILE,
    PARSE_ERROR_EXPECTED_ENDLOOP,
    PARSE_ERROR_EXPECTED_ENDIF,
    PARSE_ERROR_EXPECTED_SEMICOLON,
    PARSE_ERROR_EXPECTED_ASSIGNMENT,
    PARSE_ERROR_EXPECTED_VARIABLE_IDENTIFIER,
    PARSE_ERROR_UNDECLARED_IDENTIFIER_IN_EXPRESSION,
    PARSE_ERROR_UNDEFINED_IDENTIFIER_IN_EXPRESSION,
    PARSE_ERROR_UNDECLARED_VARIABLE_IN_ASSIGNMENT,
    PARSE_ERROR_EXPECTED_IDENTIFIER_IN_LOOP,
    PARSE_ERROR_EXPECTED_IDENTIFIER_IN_PROCEDURE,
    PARSE_ERROR_EXPECTED_IDENTIFIER_IN_PROCEDURE_PARAMS,
    PARSE_ERROR_EXPECTED_ENDPROCEDURE,
    PARSE_ERROR_PROCEDURE_IDENTIFIER_ALREADY_DECLARED,
    PARSE_ERROR_EXPECTED_IDENTIFIER_IN_FUNCTION,
    PARSE_ERROR_EXPECTED_IDENTIFIER_IN_FUNCTION_PARAMS,
    PARSE_ERROR_FUNCTION_IDENTIFIER_ALREADY_DECLARED,
    PARSE_ERROR_PROCEDURE_ARITY_MISMATCH,
    PARSE_ERROR_FUNCTION_ARITY_MISMATCH,
    PARSE_ERROR_NO_FUNCTION_RETURN_DEFINED,
    PARSE_ERROR_EXPECTED_ENDFUNCTION,
    PARSE_ERROR_SYMBOL_UNLOCATABLE,
    PARSE_ERROR_VARIABLE_REDECLARATION,
} parse_error_type;

typedef enum parse_warning_type
{
    PARSE_WARNING_SHADOWED_VARIABLE,
} parse_warning_type;

void    parser_error_handler_display_error(source_parser *parser, parse_error_type error, u64 sline);
void    parser_error_handler_display_warning(source_parser *parser, parse_warning_type warning, u64 sline);

// --- Print Traversal ---------------------------------------------------------
//
// The following print traversal is designed for viewing the raw output of the
// tree's interpretation. It's mainly used for debugging and not meant to be
// used as production code.
//

void parser_print_tree(syntax_node *root_node);

#endif
