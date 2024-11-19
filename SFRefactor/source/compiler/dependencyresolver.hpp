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
        inline std::vector<SyntaxParser*>       get_dependent_parsers() const;
        inline std::shared_ptr<DependencyNode>  get_dependency_graph()  const;

    protected:
        static inline bool resolve_recurse(std::shared_ptr<DependencyNode> current,
                SyntaxParser *parser, std::vector<SyntaxParser*> *parsers);

    protected:
        std::shared_ptr<DependencyNode> graph;
        std::vector<SyntaxParser*> parsers;
        SyntaxParser *entry;
};

inline DependencyResolver::
DependencyResolver(SyntaxParser *entry)
{

    // Set our entry parser.
    this->entry = entry;

    // The first node is our entry node.
    this->graph = std::make_shared<DependencyNode>();
    this->graph->parent = nullptr; // Entry node has no parent.
    this->graph->path   = entry->get_source_path();

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
resolve_recurse(std::shared_ptr<DependencyNode> current, SyntaxParser *parser, std::vector<SyntaxParser*> *parsers)
{

    // NOTE(Chris): The recursive method basically fetches, validates, and resolves
    //              all the dependencies and ensures no cyclic dependencies. The
    //              process constructs the graph as well as instantiates the parsers
    //              as it goes. Parsers are checked for duplicates (files can share
    //              dependencies, but not cyclic depencies) and duplicates are ignored
    //              during parser creation. This is a little convoluted, but it's the
    //              best I got for this problem.

    std::vector<std::string> paths = parser->get_includes();   
    for (const auto path : paths)
    {

        // Check the path.
        Filepath current_path(path);
        if (!current_path.is_valid_file())
        {
            
            std::cout << "[ Parser ] Path: " << path << " is not a valid file path." << std::endl;
            return false;

        }

        // Determine there is a parser for it.
        SyntaxParser *current_parser = nullptr;
        for (auto p : *parsers)
        {
            if (p->get_source_path() == current_path)
            {
                current_parser = p;
                break;
            }
        }

        // Generate a parser if one doesn't exist.
        if (current_parser == nullptr)
        {
            
            current_parser = new SyntaxParser(current_path);
            parsers->push_back(current_parser); // Introduces a new parser.

        }

        // Generate the node.
        std::shared_ptr<DependencyNode> current_node = std::make_shared<DependencyNode>();
        current_node->parent    = current;
        current_node->path      = current_path;
        current->siblings.push_back(current_node);

        std::cout << "[ Parser ] Include found: " << current_path << std::endl;
        
        bool result = resolve_recurse(current_node, current_parser, parsers);
        if (!result) return false; // If the recursion is broken, then we break.

    }

    return true;

}

inline bool DependencyResolver::
resolve()
{

    bool result = this->resolve_recurse(this->graph, this->entry, &this->parsers);
    return result;

}

inline std::vector<SyntaxParser*> DependencyResolver::
get_dependent_parsers() const
{

    return this->parsers;

}

inline std::shared_ptr<DependencyNode> DependencyResolver::
get_dependency_graph() const
{

    return this->graph;

}

#endif
