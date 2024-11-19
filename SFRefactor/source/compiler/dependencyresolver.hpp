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
                SyntaxParser *parser, std::vector<SyntaxParser*> &parsers);

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
resolve_recurse(std::shared_ptr<DependencyNode> current, SyntaxParser *parser, std::vector<SyntaxParser*> &parsers)
{

    std::vector<std::string> paths = parser->get_includes();   
    for (const auto path : paths)
    {

        // Get the path and check if it is a valid file.
        Filepath current_path(path.c_str());
        if (!current_path.is_valid_file())
        {
            
            std::cout << "[ Parser ] Path: " << path << " is not a valid file path." << std::endl;
            return false;

        }
        
        // Check for circular inclusion.
        std::shared_ptr<DependencyNode> check_node = current;
        while (check_node != nullptr)
        {

            // Circular inclusion detected, error.
            if (check_node->path == current_path)
            {
                
                std::cout << "[ Parser ] Circular inclusion detected: " << current_path << std::endl;
                return false;

            }
            
            check_node = check_node->parent;

        }

        // Fetch the parser if it already exists.
        SyntaxParser *current_parser = nullptr;
        for (int i = 0; i < parsers.size(); ++i)
        {
            Filepath reference_path = parsers.at(i)->get_source_path().c_str();
            if (reference_path == current_path)
            {
                current_parser = parsers.at(i);
                break;
            }
        }

        // Create the parser if it doesn't exist.
        if (current_parser == nullptr)
        {
            std::cout << "[ Parser ] Parser instantiated for: " << current_path << std::endl;
            current_parser = new SyntaxParser(current_path, parser);
            parsers.push_back(current_parser);
        }

        std::cout << "[ Parser ] Include found: " << current_path << std::endl;

        // Generate the node.
        std::shared_ptr<DependencyNode> current_node = std::make_shared<DependencyNode>();
        current_node->parent    = current;
        current_node->path      = current_path;
        current->siblings.push_back(current_node);
       
        bool result = resolve_recurse(current_node, current_parser, parsers);
        if (!result) return false; // If the recursion is broken, then we break.

    }

    return true;

}

inline bool DependencyResolver::
resolve()
{

    bool result = this->resolve_recurse(this->graph, this->entry, this->parsers);
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
