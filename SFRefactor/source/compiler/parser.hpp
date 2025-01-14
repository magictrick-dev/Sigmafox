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
//      global_statement            :   include_statement | procedure_statement | function_statement
//      body_statement              :   expression_statement | variable_statement | procedure_statement |
//                                      function_statement | scope_statement | while_statement |
//                                      loop_statement | if_statement | read_statement | write_statement
//
//      include_statement           :   "include" TOKEN_STRING ";" module
//      variable_statement          :   "variable" IDENTIFIER expression expression* (":=" expression)? ";"
//      scope_statement             :   "scope" (body_statement)* "endscope" ";"
//      while_statement             :   "while" expression ";" (body_statement)* "endwhile" ";"
//      loop_statement              :   "loop" identifier expression expression expression? ";" 
//                                      (body_statement)* "endloop" ";"
//      function_statement          :   "function" IDENTIFIER (IDENTIFIER)* ";" (body_statement)* "endfunction"
//      procedure_statement         :   "procedure" IDENTIFIER (IDENTIFIER)* ";" (body_statement)* "endprocedure"
//      write_statement             :   "write" expression expression ("," expression)* ";"
//      read_statement              :   "read" expression expression ";"
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
#include <compiler/nodes/while_statement.hpp>
#include <compiler/nodes/loop_statement.hpp>
#include <compiler/nodes/function_statement.hpp>
#include <compiler/nodes/procedure_statement.hpp>
#include <compiler/nodes/variable_statement.hpp>
#include <compiler/nodes/scope_statement.hpp>
#include <compiler/nodes/if_statement.hpp>
#include <compiler/nodes/conditional.hpp>
#include <compiler/nodes/read_statement.hpp>
#include <compiler/nodes/write_statement.hpp>

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
        shared_ptr<ISyntaxNode>         match_function_statement();
        shared_ptr<ISyntaxNode>         match_procedure_statement();
        shared_ptr<ISyntaxNode>         match_while_statement();
        shared_ptr<ISyntaxNode>         match_loop_statement();
        shared_ptr<ISyntaxNode>         match_if_statement();
        shared_ptr<ISyntaxNode>         match_conditional();
        shared_ptr<ISyntaxNode>         match_read_statement();
        shared_ptr<ISyntaxNode>         match_write_statement();

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

#endif