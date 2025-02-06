#ifndef SIGAMFOX_COMPILER_PARSER_PARSER_HPP
#define SIGAMFOX_COMPILER_PARSER_PARSER_HPP
#include <definitions.hpp>
#include <compiler/tokenizer.hpp>
#include <compiler/parser/node.hpp>
#include <compiler/symbols/table.hpp>

class ParseTree 
{

    public:
                    ParseTree();
        virtual    ~ParseTree();

        bool        parse(string source_file);
        bool        valid() const;

    protected:
        void synchronize_to(TokenType type);
        void synchronize_up_to(TokenType type);

        template <class T, typename ...Params> std::shared_ptr<T> generate_node(Params... args);

        shared_ptr<SyntaxNode> match_root();
        shared_ptr<SyntaxNode> match_module();

        shared_ptr<SyntaxNode> match_global_statement();
        shared_ptr<SyntaxNode> match_include_statement();
        shared_ptr<SyntaxNode> match_function_statement(bool is_global);
        shared_ptr<SyntaxNode> match_procedure_statement(bool is_global);
        shared_ptr<SyntaxNode> match_main_statement();

        shared_ptr<SyntaxNode> match_local_statement();
        shared_ptr<SyntaxNode> match_expression_statement();
        shared_ptr<SyntaxNode> match_variable_statement();
        shared_ptr<SyntaxNode> match_scope_statement();
        shared_ptr<SyntaxNode> match_while_statement();
        shared_ptr<SyntaxNode> match_loop_statement();
        shared_ptr<SyntaxNode> match_conditional_if_statement();
        shared_ptr<SyntaxNode> match_conditional_elseif_statement();
        shared_ptr<SyntaxNode> match_read_statement();
        shared_ptr<SyntaxNode> match_write_statement();
        shared_ptr<SyntaxNode> match_procedure_call_statement();

        shared_ptr<SyntaxNode> match_expression();
        shared_ptr<SyntaxNode> match_assignment();
        shared_ptr<SyntaxNode> match_equality();
        shared_ptr<SyntaxNode> match_comparison();
        shared_ptr<SyntaxNode> match_term();
        shared_ptr<SyntaxNode> match_factor();
        shared_ptr<SyntaxNode> match_magnitude();
        shared_ptr<SyntaxNode> match_extraction();
        shared_ptr<SyntaxNode> match_derivation();
        shared_ptr<SyntaxNode> match_unary();
        shared_ptr<SyntaxNode> match_function_call();
        shared_ptr<SyntaxNode> match_array_index();
        shared_ptr<SyntaxNode> match_primary();

    protected:
        vector<shared_ptr<SyntaxNode>> nodes;
        shared_ptr<SyntaxNode> root;

};

#endif