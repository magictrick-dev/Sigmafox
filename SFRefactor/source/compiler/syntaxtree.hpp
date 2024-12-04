#ifndef SIGMAFOX_COMPILER_SYNTAX_TREE
#define SIGMAFOX_COMPILER_SYNTAX_TREE
#include <definitions.hpp>
#include <utilities/path.hpp>
#include <compiler/parser.hpp>
#include <compiler/dependencygraph.hpp>

class SyntaxTree
{

    public:
                        SyntaxTree();
        virtual        ~SyntaxTree();

        bool            construct_ast(Filepath entry_file);
        void            visit_root(ISyntaxNodeVisitor *visitor);

    protected:
        shared_ptr<ISyntaxNode> root;
        DependencyGraph graph;

};

#endif
