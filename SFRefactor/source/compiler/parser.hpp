// --- Sigmafox Compiler Parser ------------------------------------------------
//
// Written by Chris DeJong Nov. 2024 / Northern Illinois University
//
//      Constructs an AST based on a given entry-source file. The grammar for
//      the language is described in detail below.
//
// --- Language Grammar --------------------------------------------------------
//
//      The language grammar is written below. The parser is a hand-written
//      recursive descent parser, so be sure to study this grammar before diving
//      into the parser itself.
//
//      root                        :   (global_statement)* main EOF
//      module                      :   (global_statement)* EOF
//
//      main                        :   "begin" ";" (body_statement)* "end" ";"
//
//      global_statement            :   include_statement | procedure_statement
//      body_statement              :   expression_statement | variable_statement | procedure_statement
//
//      include_statement           :   "include" TOKEN_STRING ";" module
//      variable_statement          :   "variable" IDENTIFIER expression expression* (":=" expression)? ";"
//      scope_statement             :   "scope" (body_statement)* "endscope"
//      procedure_statement         :   "procedure" IDENTIFIER (IDENTIFIER)* ";" (body_statement)* "endprocedure"
//      expression_statement        :   expression ";"
//
//      expression                  :   procedure_call
//      procedure_call              :   IDENTIFIER "(" arguments? ")"
//      assigment                   :   IDENTIFIER ":=" assignment | equality
//      equality                    :   comparison (("=" | "#") comparison)*
//      comparison                  :   term (("<" | "<=" | ">" | ">=") term)*
//      term                        :   factor (("+" | "-") factor)*
//      factor                      :   magnitude (("*" | "/") magnitude)*
//      magnitude                   :   extraction ("^" extraction)*
//      extraction                  :   derivation ("|" derivation)*
//      derivation                  :   unary ("%" unary)*
//      unary                       :   ("-" unary) | call
//      function_call               :   primary ( "(" arguments? ")" )?
//      array_index                 :   IDENTIFIER "(" expression ("," expression)* ")"
//      primary                     :   NUMBER | STRING | IDENTIFIER | "(" expression ")"
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_COMPILER_PARSER_H 
#define SIGMAFOX_COMPILER_PARSER_H 
#include <vector>
#include <exception>
#include <definitions.hpp>
#include <utilities/path.hpp>

#include <compiler/symbolstack.hpp>
#include <compiler/tokenizer.hpp>
#include <compiler/dependencygraph.hpp>
#include <compiler/errorhandler.hpp>
#include <compiler/syntaxnode.hpp>

#include <compiler/nodes/module.hpp>
#include <compiler/nodes/root.hpp>

#include <compiler/nodes/include.hpp>

#include <compiler/nodes/main.hpp>
#include <compiler/nodes/expression_statement.hpp>
#include <compiler/nodes/procedure_statement.hpp>
#include <compiler/nodes/variable_statement.hpp>
#include <compiler/nodes/scope_statement.hpp>

#include <compiler/nodes/assignment.hpp>
#include <compiler/nodes/procedure_call.hpp>
#include <compiler/nodes/equality.hpp>
#include <compiler/nodes/comparison.hpp>
#include <compiler/nodes/term.hpp>
#include <compiler/nodes/factor.hpp>
#include <compiler/nodes/magnitude.hpp>
#include <compiler/nodes/extraction.hpp>
#include <compiler/nodes/derivation.hpp>
#include <compiler/nodes/unary.hpp>
#include <compiler/nodes/function_call.hpp>
#include <compiler/nodes/array_index.hpp>
#include <compiler/nodes/primary.hpp>
#include <compiler/nodes/grouping.hpp>

class SyntaxParser
{

    public:
                        SyntaxParser(Filepath filepath, DependencyGraph* graph);
        virtual        ~SyntaxParser();

        bool            construct_as_root();
        bool            construct_as_module();

        void            visit_base_node(ISyntaxNodeVisitor *visitor);

        inline shared_ptr<ISyntaxNode> get_base_node() const { return this->base_node; }

        Filepath        get_source_path() const;

    protected:
        void synchronize_to(TokenType type);
        void synchronize_up_to(TokenType type);
        template <class T, typename ...Params> std::shared_ptr<T> generate_node(Params... args);

        shared_ptr<ISyntaxNode>         match_root();
        shared_ptr<ISyntaxNode>         match_module();

        shared_ptr<ISyntaxNode>         match_main();

        shared_ptr<ISyntaxNode>         match_global_statement();
        shared_ptr<ISyntaxNode>         match_include_statement();

        shared_ptr<ISyntaxNode>         match_body_statement();
        shared_ptr<ISyntaxNode>         match_expression_statement();
        shared_ptr<ISyntaxNode>         match_variable_statement();
        shared_ptr<ISyntaxNode>         match_scope_statement();
        shared_ptr<ISyntaxNode>         match_procedure_statement();

        shared_ptr<ISyntaxNode>         match_expression();
        shared_ptr<ISyntaxNode>         match_procedure_call();
        shared_ptr<ISyntaxNode>         match_assignment();
        shared_ptr<ISyntaxNode>         match_equality();
        shared_ptr<ISyntaxNode>         match_comparison();
        shared_ptr<ISyntaxNode>         match_term();
        shared_ptr<ISyntaxNode>         match_factor();
        shared_ptr<ISyntaxNode>         match_magnitude();
        shared_ptr<ISyntaxNode>         match_extraction();
        shared_ptr<ISyntaxNode>         match_derivation();
        shared_ptr<ISyntaxNode>         match_unary();
        shared_ptr<ISyntaxNode>         match_function_call();
        shared_ptr<ISyntaxNode>         match_array_index();
        shared_ptr<ISyntaxNode>         match_primary();

        template<TokenType expect> void validate_grammar_token();
        bool expect_previous_token_as(TokenType type) const;
        bool expect_current_token_as(TokenType type) const;
        bool expect_next_token_as(TokenType type) const;

        void process_error(i32 where, SyntaxException& error, bool just_handled);
        void process_warning(i32 where, SyntaxException& warning, bool just_handled);

    protected:
        std::vector<shared_ptr<ISyntaxNode>>    nodes;
        shared_ptr<ISyntaxNode>                 base_node;
        SymboltableStack<Symbol>                symbol_stack;
        DependencyGraph*                        graph;
        Tokenizer                               tokenizer;
        Filepath                                path;
        i32                                     error_count;

};

/*
#ifndef SOURCE_COMPILER_RPARSER_H
#define SOURCE_COMPILER_RPARSER_H
#include <core/definitions.h>
#include <core/arena.h>
#include <compiler/stringpool.h>
#include <compiler/symboltable.h>
#include <compiler/tokenizer.h>
#include <compiler/errorhandler.h>

#define STRING_POOL_DEFAULT_SIZE    SF_MEGABYTES(256)
#define SYMBOL_TABLE_DEFAULT_SIZE   SF_MEGABYTES(256)

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
    ARRAY_INDEX_EXPRESSION_NODE,

    EXPRESSION_STATEMENT_NODE,
    INCLUDE_STATEMENT_NODE,

    COMMENT_STATEMENT_NODE,
    NEWLINE_STATEMENT_NODE,
    DECLARATION_STATEMENT_NODE,
    SCOPE_STATEMENT_NODE,
    LOOP_STATEMENT_NODE,
    WHILE_STATEMENT_NODE,
    IF_STATEMENT_NODE,
    ELSEIF_STATEMENT_NODE,
    WRITE_STATEMENT_NODE,
    READ_STATEMENT_NODE,
    PROCEDURE_STATEMENT_NODE,
    FUNCTION_STATEMENT_NODE,
    PARAMETER_STATEMENT_NODE,
    VARIABLE_STATEMENT_NODE,
    MODULE_ROOT_NODE,
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

typedef struct object_literal
{

    union {
        u64 unsigned_integer;
        i64 signed_integer;
        b64 boolean;
        r64 real;
    };

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
    ccptr identifier;
    syntax_node *left;
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

typedef struct array_index_syntax_node
{
    
    cc64 name;
    syntax_node *accessors;

} array_index_syntax_node;

typedef struct expression_syntax_node
{

    syntax_node *expression;

} expression_syntax_node;

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

typedef struct write_syntax_node
{
    syntax_node *location;
    syntax_node *body_expressions;
} write_syntax_node;

typedef struct read_syntax_node
{
    syntax_node *location;
    cc64 identifier;
} read_syntax_node;

typedef struct parameter_syntax_node
{
    cc64 name;
    syntax_node *next_parameter;
} parameter_syntax_node;

typedef struct include_syntax_node
{
    cc64 file_path;
} include_syntax_node;

typedef struct global_statement_syntax_node
{
    syntax_node *statement;
} global_statement_syntax_node;

typedef struct program_syntax_node
{
    syntax_node *global_statements;
    syntax_node *body_statements;
} program_syntax_node;

typedef struct module_syntax_node
{
    syntax_node *global_statements;
} module_syntax_node;

typedef struct syntax_node
{

    syntax_node_type type;
    syntax_node *next_node;

    union
    {
        binary_syntax_node          binary;
        unary_syntax_node           unary;
        primary_syntax_node         primary;
        grouping_syntax_node        grouping;
        assignment_syntax_node      assignment;
        variable_syntax_node        variable;
        scope_syntax_node           scope;
        while_syntax_node           while_loop;
        loop_syntax_node            for_loop;
        if_syntax_node              if_conditional;
        elseif_syntax_node          elseif_conditional;
        procedure_syntax_node       procedure;
        function_syntax_node        function;
        parameter_syntax_node       parameter;
        program_syntax_node         program;
        write_syntax_node           write;
        read_syntax_node            read;
        include_syntax_node         include;
        array_index_syntax_node     array_index;
        expression_syntax_node      expression;
        module_syntax_node          module;

        procedure_call_syntax_node          proc_call;
        function_call_syntax_node           func_call;

    };

} syntax_node;

typedef struct parser_tokenizer parser_tokenizer;

typedef struct parser_tokenizer
{

    source_tokenizer tokenizer;
    source_token tokens[3];
    source_token *previous_token;
    source_token *current_token;
    source_token *next_token;

    parser_tokenizer *parent_tokenizer;

} parser_tokenizer;

typedef struct source_parser
{

    parser_tokenizer *tokenizer;

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
syntax_node* source_parser_match_array_index(source_parser *parser);
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
syntax_node* source_parser_match_write_statement(source_parser *parser);
syntax_node* source_parser_match_statement(source_parser *parser);
syntax_node* source_parser_match_global_statement(source_parser *parser);
syntax_node* source_parser_match_import_statement(source_parser *parser);
syntax_node* source_parser_match_module(ccptr file_name, source_parser *parser);
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

void source_parser_push_tokenizer(source_parser *parser, ccptr file_path, cptr source);
void source_parser_pop_tokenizer(source_parser *parser);

// --- Symbol Table Helpers ----------------------------------------------------

void source_parser_push_symbol_table(source_parser *parser);
void source_parser_pop_symbol_table(source_parser *parser);
symbol* source_parser_insert_into_symbol_table(source_parser *parser, cc64 identifier);
symbol* source_parser_locate_symbol(source_parser *parser, cc64 identifer);
b32 source_parser_identifier_is_declared(source_parser *parser, cc64 identifier);
b32 source_parser_identifier_is_declared_in_scope(source_parser *parser, cc64 identifier);
b32 source_parser_identifier_is_declared_above_scope(source_parser *parser, cc64 identifier);
b32 source_parser_identifier_is_defined(source_parser *parser, cc64 identifier);

// --- Print Traversal ---------------------------------------------------------
//
// The following print traversal is designed for viewing the raw output of the
// tree's interpretation. It's mainly used for debugging and not meant to be
// used as production code.
//

void parser_print_tree(syntax_node *root_node);
*/

#endif
