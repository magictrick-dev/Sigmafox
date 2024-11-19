// --- Sigmafox Compiler Dependency Resolver -----------------------------------
//
// Written by Chris DeJong Nov. 2024 / Northern Illinois University
//
//
// -----------------------------------------------------------------------------
#ifndef SIGMAFOX_COMPILER_DEPENDENCYGRAPH_HPP
#define SIGMAFOX_COMPILER_DEPENDENCYGRAPH_HPP
#include <vector>
#include <definitions.hpp>
#include <utilities/path.hpp>

struct DependencyNode
{
    std::vector<std::shared_ptr<DependencyNode>> siblings;
    std::shared_ptr<DependencyNode> parent;
    Filepath path;
};

class DependencyGraph
{

    public:
        inline          DependencyGraph();
        inline virtual ~DependencyGraph();

        std::shared_ptr<DependencyNode> base_node;

};

#endif
