#ifndef SIGAMFOX_COMPILER_PARSER_PARSER_HPP
#define SIGAMFOX_COMPILER_PARSER_PARSER_HPP
#include <definitions.hpp>
#include <compiler/graph.hpp>
#include <compiler/environment.hpp>
#include <compiler/parser/node.hpp>
#include <compiler/tokenizer/tokenizer.hpp>

class ParseTree 
{

    public:
                    ParseTree(DependencyGraph* graph, Environment* environment);
        virtual    ~ParseTree();

                    ParseTree(const ParseTree&) = delete; // None of that nonsense.
        ParseTree&  operator=(const ParseTree&) = delete; // Or this nonsense.

        bool        parse(string source_file);
        bool        valid() const;

        SyntaxNode*             get_root();
        vector<SyntaxNode*>&    get_nodes();

    protected:
        void synchronize_to(Tokentype type);
        void synchronize_up_to(Tokentype type);

        bool expect_current_token_as(Tokentype type) const;
        bool expect_next_token_as(Tokentype type) const;

        void consume_current_token_as(Tokentype type, u64 sloc);

        template <class T, typename ...Params> T* generate_node(Params... args);

        SyntaxNode* match_root();

        SyntaxNode* match_global_statement();
        SyntaxNode* match_include_statement();
        SyntaxNode* match_function_statement(bool is_global);
        SyntaxNode* match_procedure_statement(bool is_global);
        SyntaxNode* match_begin_statement();

        SyntaxNode* match_local_statement();
        SyntaxNode* match_expression_statement();
        SyntaxNode* match_variable_statement();
        SyntaxNode* match_scope_statement();
        SyntaxNode* match_while_statement();
        SyntaxNode* match_loop_statement();
        SyntaxNode* match_conditional_if_statement();
        SyntaxNode* match_conditional_elseif_statement();
        SyntaxNode* match_read_statement();
        SyntaxNode* match_write_statement();

        SyntaxNode* match_expression();
        SyntaxNode* match_procedure_call();
        SyntaxNode* match_assignment();
        SyntaxNode* match_equality();
        SyntaxNode* match_comparison();
        SyntaxNode* match_concatenation();
        SyntaxNode* match_term();
        SyntaxNode* match_factor();
        SyntaxNode* match_magnitude();
        SyntaxNode* match_extraction();
        SyntaxNode* match_derivation();
        SyntaxNode* match_unary();
        SyntaxNode* match_function_call();
        SyntaxNode* match_array_index();
        SyntaxNode* match_primary();

    protected:
        DependencyGraph*                graph;
        Environment*                    environment;
        shared_ptr<Tokenizer>           tokenizer;
        
    protected:
        Filepath                        path;
        SyntaxNode*                     root;
        vector<SyntaxNode*>             nodes;

};

#endif
