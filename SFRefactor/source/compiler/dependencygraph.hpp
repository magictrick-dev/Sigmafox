// --- Sigmafox Compiler Dependency Graph --------------------------------------
//
// Written by Chris DeJong Nov. 2024 / Northern Illinois University
//
//      The dependency graph maintains the list of file includes, starting from
//      the entry file outwards. It allows the syntax tree to add new dependencies,
//      check for circular includes, and construct and merge symbol tables as they're
//      added into the dependency chain.
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_COMPILER_DEPENDENCYGRAPH_HPP
#define SIGMAFOX_COMPILER_DEPENDENCYGRAPH_HPP
#include <vector>
#include <string>
#include <unordered_map>
#include <definitions.hpp>
#include <utilities/path.hpp>

// Forward declare so we can feed this to the nodes.
class SyntaxParser;

// --- Dependency Node ---------------------------------------------------------
//
// A dependency node is dependency in the source heirarchy. Nodes represent a
// path as well as a reference to their parent and siblings.
//

class DependencyNode
{

    public:
                    DependencyNode();
        virtual    ~DependencyNode();

};

// --- Dependency Graph --------------------------------------------------------
//
// A dependency graph begins with an entry node, provided by the syntax tree that
// owns this object.
//

class DependencyGraph
{

    public:
                    DependencyGraph();
        virtual    ~DependencyGraph();

    protected:
        shared_ptr<DependencyNode> entry_node;
        std::unordered_map<std::string, shared_ptr<DependencyNode>> node_map;
        std::vector<shared_ptr<DependencyNode>> node_list;

};

#endif
