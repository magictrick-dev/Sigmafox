#ifndef SOURCE_COMPILER_RPARSER_H
#define SOURCE_COMPILER_RPARSER_H
#include <core/definitions.h>

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
    cc64 source;
    u64 offset;
    u64 step;
} source_tokenizer;

typedef enum source_token_type
{
    TOKEN_COMMENT_BLOCK,
    TOKEN_NEW_LINE,
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
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD_BEGIN,
    TOKEN_KEYWORD_END,
    TOKEN_KEYWORD_PROCEDURE,
    TOKEN_KEYWORD_ENDPROCEDURE,
    TOKEN_KEYWORD_FUNCTION,
    TOKEN_KEYWORD_ENDFUNCTION,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_ELSEIF,
    TOKEN_KEYWORD_ENDIF,
    TOKEN_KEYWORD_WHILE,
    TOKEN_KEYWORD_ENDWHILE,
    TOKEN_KEYWORD_LOOP,
    TOKEN_KEYWORD_ENDLOOP,
    TOKEN_KEYWORD_PLOOP,
    TOKEN_KEYWORD_ENDPLOOP,
    TOKEN_KEYWORD_FIT,
    TOKEN_KEYWORD_ENDFIT,
    TOKEN_KEYWORD_VARIABLE,
    TOKEN_KEYWORD_WRITE,
    TOKEN_KEYWORD_READ,
    TOKEN_KEYWORD_SAVE,
    TOKEN_KEYWORD_INCLUDE,
    TOKEN_KEYWORD_SCOPE,
    TOKEN_KEYWORD_ENDSCOPE,
    TOKEN_UNDEFINED,
    TOKEN_UNDEFINED_EOF,
    TOKEN_UNDEFINED_EOL,
    TOKEN_EOF,
} source_token_type;

typedef struct source_token
{
    source_token_type type;
    cc64 source;
    u64 offset;
    u64 length;
} source_token;

cc64    source_tokenizer_token_type_to_string(source_token *token);
b32     source_tokenizer_eof(source_tokenizer *tokenizer);
b32     source_tokenizer_eol(source_tokenizer *tokenizer);
b32     source_tokenizer_match(source_tokenizer *tokenizer, u32 count, ...);
char    source_tokenizer_peek(source_tokenizer *tokenizer, u64 offset);
char    source_tokenizer_consume(source_tokenizer *tokenizer, u64 count);
void    source_tokenizer_set_token(source_tokenizer *tokenizer, source_token *token, source_token_type type);
void    source_tokenizer_synchronize(source_tokenizer *tokenizer);
b32     source_tokenizer_consume_whitespace(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_comments(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_newline(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_symbols(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_numbers(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_strings(source_tokenizer *tokenizer, source_token *token);
b32     source_tokenizer_match_identifiers(source_tokenizer *tokenizer, source_token *token);
void    source_tokenizer_get_next_token(source_tokenizer *tokenizer, source_token *token);
void    source_tokenizer_initialize(source_tokenizer *tokenizer, cc64 source, cc64 path);

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
//                                  comment_statement | newline_statement
//      statement               :   declaration_statement | expression_statement | invoke_statement
//                                  scope_statement | while_statement | loop_statement
//                                  conditional_statement | procedure_statement | function_statement
//                                  comment_statement | newline_statement
//      comment_statement       :   "{" * "}"
//      newline_statement       :   "\n"
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
    BINARY_EXPRESSION_NODE,
    UNARY_EXPRESSION_NODE,
    PRIMARY_EXPRESSION_NODE,
    CALL_EXPRESSION_NODE,
    GROUPING_EXPRESSION_NODE,

    EXPRESSION_STATEMENT_NODE,
    COMMENT_STATEMENT_NODE,
    NEWLINE_STATEMENT_NODE,
    DECLARATION_STATEMENT_NODE,
    SCOPE_STATEMENT_NODE,
    LOOP_STATEMENT_NODE,
    WHILE_STATEMENT_NODE,
    IF_STATEMENT_NODE,
    ELSEIF_STATEMENT_NODE,
    PROCEDURE_STATEMENT_NODE
} syntax_node_type;

typedef enum syntax_operation_type
{
    OPERATION_ADDITION,
    OPERATION_SUBTRACTION,
    OPERATION_MULTIPLICATION,
    OPERATION_DIVISION,
    OPERATION_NEGATIVE_ASSOCIATE,
} syntax_operation_type;

typedef union object_literal
{
    u64 unsigned_integer;
    i64 signed_integer;
    b64 boolean;
    r64 real;
    cc64 string;
    cc64 identifier;
    cc64 procedure_name;
} object_literal;

typedef enum object_type
{
    OBJECT_UNSIGNED_INTEGER,
    OBJECT_SIGNED_INTEGER,
    OBJECT_BOOLEAN,
    OBJECT_REAL,
    OBJECT_STRING,
    OBJECT_PROCEDURE,
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

typedef struct parameter_syntax_node
{
    syntax_node *expression;
    syntax_node *next_parameter;
} parameter_syntax_node;

typedef struct call_syntax_node 
{
    object_literal call_identifier;
    object_type call_type;
    syntax_node *parameter_list;
    u64 parameter_count; 
} call_syntax_node;

typedef struct assigment_syntax_node
{
    syntax_node *expression;
    object_literal literal;
    object_type type;
} assignment_syntax_node;

typedef struct syntax_node
{

    syntax_node_type type;

    union
    {
        binary_syntax_node      binary;
        unary_syntax_node       unary;
        primary_syntax_node     primary;
        grouping_syntax_node    grouping;
        call_syntax_node        call;
        parameter_syntax_node   parameter;
        assignment_syntax_node  assignment;
    };

} syntax_node;

typedef struct source_parser
{
    source_tokenizer tokenizer;
    source_token tokens[3];
    source_token *previous_token;
    source_token *current_token;
    source_token *next_token;

    syntax_node *entry;
    syntax_node *nodes;
    u64 node_count;
} source_parser;

syntax_node*    source_parser_create_ast(source_parser *parser);

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
    PARSE_ERROR_UNDEFINED_TOKEN,
    PARSE_ERROR_UNEXPECTED_EOL,
    PRASE_ERROR_UNEXPECTED_EOF,
} parse_error_type;

typedef enum parse_warning_type
{
    PARSE_WARNING_SHADOWED_VARIABLE,
} parse_warning_type;

void    parser_error_handler_display_error(source_parser *parser, parse_error_type error);
void    parser_error_handler_display_warning(source_parser *parser, parse_warning_type warning);
void    parser_error_handler_synchronize(source_parser *parser);
b32     parser_error_handler_synchronize_to(source_parser *parser, source_token_type type);

#endif
