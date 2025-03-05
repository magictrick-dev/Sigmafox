#ifndef SIGAMFOX_COMPILER_PARSER_PARSER_HPP
#define SIGAMFOX_COMPILER_PARSER_PARSER_HPP
#include <definitions.hpp>
#include <compiler/graph.hpp>
#include <compiler/environment.hpp>
#include <compiler/parser/node.hpp>
#include <compiler/tokenizer/tokenizer.hpp>

// Notes and thoughts.
//
//      1.  Since we are using shared pointers, the symbol table itself can't interface
//          with the nodes directly due to the fact that visitor interfaces directly
//          call the "this" pointer, which means we can't let the symbol table use
//          shared pointers. There are work arounds for this, but for now we just
//          turn off shared pointers on the symbol table to ensure this works.
//
//              a.  Avoid the idea of shared pointers and instead go back to heap
//                  allocated nodes and ensure that they're cleaned up at the end of
//                  the parse tree's life cycle. Since we dynamically create additional
//                  parse branches and gobble the old nodes, this system would work so
//                  long as we don't forget to remove the nodes from their parent list.
//                  
//              b.  Bonus points: we can track how much memory we're utilizing by tracking
//                  the number of nodes we have in the parse tree.
//
//      2.  The expression and block validators can adequately deduce the return types.
//          Most of the time, nodes want to know what their originating identifier was
//          and we don't really feed this information in at the time of creation. For
//          example, variable nodes contain identifiers for the LHS, but we don't track
//          this for assignment nodes. We have to perform a few indirections just to
//          retrieve this information and isn't ideal when we can just feed this information
//          back during the creation of the node.
//          
//              a.  Promote our assignment nodes to contain the originating identifier.
//                  
//              b.  Why aren't we tracking the originating *location* of the node? Error
//                  handling would be so much simpler since we can just store a reference
//                  point of where it came from.
//          
//      3.  The issue of direct and indirect recursion doesn't seem possible given
//          the rules of the language. For example, we parse and validate the functions
//          procedurally, so we can't have a function call a function that hasn't been
//          defined yet. Additionally, since the return value of a function is the name
//          of the function, we effectively shadowing the name of the function, so we
//          can't have a function call itself recursively. Two birds, one stone, except
//          there's no stone and it was all an accident.
//          
//              a.  Do we care? Perhaps extend the language to add a return statement...
//
//      4.  Error messages inside the validators should be able to propagate back up
//          to the parser, but what I have done is tied the token to the error message.
//          The validators don't have access to the token and the nodes don't either.
//          We will need a better error handling system to ensure that we can properly
//          propagate error messages back up.
//          
//              a.  CompilerException base class, robbed from my CSCI-515 class.
//              b.  CompilerSyntaxError for syntax errors.
//              c.  CompilerSyntaxWarning for syntax warnings.
//              d.  CompilerSyntaxInformation for syntax notes.
//              
//              h.  Propagation of errors can be handled at point of origin. Generally,
//                  we synchronize bottom up, so we can continue to throw until we reach
//                  a satisfactory point.
//                  
//              i.  Leads into the question of how we track if we encountered an error.
//                  The parser doesn't track this information, it will always be valid,
//                  regardless of how many errors we have. What we can do is have an error
//                  flag that is set when an error is thrown and then we can check this
//                  flag at the end of the parse tree's life cycle. Simple, but effective.
//
//      5.  Obvious bug here, but if a symbol isn't found, we crash.
//      
//      6.  The assignment parser has enough bugs in it to be considered an infestation.
//          
//              a.  We're probably not accurately handling arrays and eventually
//                  we will want to extend this functionality to vectors as well.
//                  
//              b.  The most obvious fix here is to just rewrite this routine and
//                  break it down into smaller parts for each case.
//      

class ParseTree 
{

    public:
                    ParseTree(DependencyGraph* graph, Environment* environment);
        virtual    ~ParseTree();

        bool        parse(string source_file);
        bool        valid() const;

        shared_ptr<SyntaxNode>              get_root() const;
        vector<shared_ptr<SyntaxNode>>      get_nodes() const;

    protected:
        void synchronize_to(TokenType type);
        void synchronize_up_to(TokenType type);

        bool expect_current_token_as(TokenType type) const;
        bool expect_next_token_as(TokenType type) const;

        void consume_current_token_as(TokenType type, u64 sloc);

        template <class T, typename ...Params> std::shared_ptr<T> generate_node(Params... args);

        shared_ptr<SyntaxNode> match_root();

        shared_ptr<SyntaxNode> match_global_statement();
        shared_ptr<SyntaxNode> match_include_statement();
        shared_ptr<SyntaxNode> match_function_statement(bool is_global);
        shared_ptr<SyntaxNode> match_procedure_statement(bool is_global);
        shared_ptr<SyntaxNode> match_begin_statement();

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
        Filepath                        path;
        DependencyGraph*                graph;
        Environment*                    environment;
        shared_ptr<Tokenizer>           tokenizer;
        shared_ptr<SyntaxNode>          root;
        vector<shared_ptr<SyntaxNode>>  nodes;

};

#endif