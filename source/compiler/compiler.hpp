#ifndef SIGMAFOX_COMPILER_COMPILER_HPP
#define SIGMAFOX_COMPILER_COMPILER_HPP
#include <definitions.hpp>
#include <compiler/environment.hpp>
#include <compiler/graph.hpp>
#include <compiler/parser/node.hpp>

class Compiler
{

    public:
                    Compiler(string entry_file);
        virtual    ~Compiler();

        bool        parse();
        bool        validate() const;
        bool        generate() const;

    protected:
        DependencyGraph             graph;
        Environment                 environment;
        SyntaxNode*                 root;
        std::vector<SyntaxNode*>    nodes;

};

#endif