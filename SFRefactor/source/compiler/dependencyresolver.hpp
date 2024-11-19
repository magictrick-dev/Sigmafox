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
#include <utilities/resourcemanager.hpp>
#include <compiler/parser.hpp>
#include <compiler/dependencygraph.hpp>

class DependencyResolver
{

    public:
        inline          DependencyResolver(SyntaxParser *entry);
        inline virtual ~DependencyResolver();

        inline bool     resolve();
        inline std::vector<SyntaxParser*>   get_dependent_parsers() const;
        inline DependencyNode               get_dependency_graph()  const;

    protected:
        static inline bool resolve_recurse(DependencyNode* current,
                SyntaxParser *parser, std::vector<SyntaxParser*> *parsers);

    protected:
        DependencyNode  graph;
        SyntaxParser   *entry;
        std::vector<SyntaxParser*> parsers;
};

inline DependencyResolver::
DependencyResolver(SyntaxParser *entry)
{

    // Set our entry parser.
    this->entry = entry;

    // The first node is our entry node.
    this->graph.parent = nullptr; // Entry node has no parent.
    this->graph.path = entry->get_source_path();

    // Place our entry node in the list.
    this->parsers.push_back(entry);

    return;

}

inline DependencyResolver::
~DependencyResolver()
{

    return;

}

inline bool DependencyResolver::
resolve_recurse(DependencyNode *current, SyntaxParser *parser, std::vector<SyntaxParser*> *parsers)
{

    // NOTE(Chris): The recursive method basically fetches, validates, and resolves
    //              all the dependencies and ensures no cyclic dependencies. The
    //              process constructs the graph as well as instantiates the parsers
    //              as it goes. Parsers are checked for duplicates (files can share
    //              dependencies, but not cyclic depencies) and duplicates are ignored
    //              during parser creation. This is a little convoluted, but it's the
    //              best I got for this problem.

    std::vector<std::string> paths = parser->get_includes();   
    for (auto p : paths)
    {
        std::cout << p << std::endl;
    }

    return false;

}

inline bool DependencyResolver::
resolve()
{

    bool result = this->resolve_recurse(&this->graph, this->entry, &this->parsers);
    return result;

}

inline std::vector<SyntaxParser*> DependencyResolver::
get_dependent_parsers() const
{

    return this->parsers;

}

inline DependencyNode DependencyResolver::
get_dependency_graph() const
{

    return this->graph;

}

#endif
