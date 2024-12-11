// --- Sigmafox Compiler Dependency Resolver -----------------------------------
//
// Written by Chris DeJong Nov. 2024 / Northern Illinois University
//
//      The dependency resolver is the first stage of the compiler which constructs
//      a well-formed dependency graph given a entry-point file. The final result
//      is a complete list of files that need to be parsed and the order to parse
//      them in. The setup is complex and requires a lot of work to ensure that
//      everything works correctly.
//
//      IMPORTANT - PLEASE READ CAREFULLY
//
//          The dependency resolver preserves the state of the sub-parsers and shouldn't
//          go out of scope lest the dependency parsers get yoinked. This is your WARNING
//          to maintain the dependency resolver UNTIL you have complete the AST parsing
//          OR the resolver fails to resolve dependencies. The entry parser requires
//          the dependency graph and the list of sub-parsers provided by this class to
//          properly parse everything. 
//
// -----------------------------------------------------------------------------
#ifndef SIGAMFOX_COMPILER_DEPENDENCYRESOLVER_HPP
#define SIGAMFOX_COMPILER_DEPENDENCYRESOLVER_HPP
#include <definitions.hpp>
#include <utilities/path.hpp>
#include <compiler/parser.hpp>
#include <compiler/dependencygraph.hpp>

class DependencyResolver
{

    public:
                    DependencyResolver(Filepath entry);
                    DependencyResolver(SyntaxParser *entry);
        virtual    ~DependencyResolver();

        bool                            resolve();
        std::vector<SyntaxParser*>      get_dependent_parsers() const;
        std::shared_ptr<DependencyNode> get_dependency_graph()  const;

    protected:
        static bool resolve_recurse(std::shared_ptr<DependencyNode> current,
                SyntaxParser *parser, std::vector<SyntaxParser*> &parsers);

    protected:
        std::shared_ptr<DependencyNode> graph;
        std::vector<SyntaxParser*> parsers;
        SyntaxParser *entry;
};

#endif
