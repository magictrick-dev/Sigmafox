// --- Sigmafox Compiler Dependency Graph --------------------------------------
//
// Written by Chris DeJong Nov. 2024 / Northern Illinois University
//
//      The dependency graph is the contained list of the files needed to generate
//      a complete AST. The graph's responsibility is to construct a level-order
//      list of files to parse, in order, by the syntax parser.
//
//      Considerations:
//
//          1.  Since we want to process leaf nodes first.
//          2.  The symbol tables must be merged, but constructed dynamically.
//              This means that we can't collapse the tables until we know all
//              symbols. Each table has their own globals, but global searches
//              must resolve across the full breadth of all tables.
//          3.  We are allowed to collapse all tables required for the parse into
//              one, statically referenceable tree, but we must maintain a seperate
//              table that corresponds *only* for that parser node.
//          4.  We can maintain these tables OR just dynamically restore them.
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_COMPILER_DEPENDENCYGRAPH_HPP
#define SIGMAFOX_COMPILER_DEPENDENCYGRAPH_HPP
#include <vector>
#include <definitions.hpp>
#include <utilities/path.hpp>

// --- Dependency Node ---------------------------------------------------------
//
// A dependency node is dependency in the source heirarchy. Nodes represent a
// path as well as a reference to their parent and siblings. They're primarily
// used in determining if there are circular inclusions and establishing a parse
// order based on the layout of the include dependency graph.
//

class DependencyNode
{
    public:
                    DependencyNode(Filepath path, std::shared_ptr<DependencyNode> parent);
        virtual    ~DependencyNode();

        std::shared_ptr<DependencyNode>     get_parent() const;
        Filepath                            get_path() const;
        std::string                         get_path_as_string() const;

    public:
        std::vector<std::shared_ptr<DependencyNode>> siblings;

    protected:
        std::shared_ptr<DependencyNode> parent;
        Filepath path;
};

// --- Dependency Graph --------------------------------------------------------
//
// The dependency graph contains the tree list of all the dependencies. A graph
// is constructed by the dependency resolver which checks and ensures there are
// not circular dependencies.
//
// A graph begins with the entry file, or the "main" compilation unit passed
// into the compiler. This file is generally the one the user passes in the CLI.
//

class DependencyGraph
{

    public:
                    DependencyGraph();
        virtual    ~DependencyGraph();

        std::shared_ptr<DependencyNode> get_base_node() const;

    protected:
        std::shared_ptr<DependencyNode> base_node;

};

#endif
