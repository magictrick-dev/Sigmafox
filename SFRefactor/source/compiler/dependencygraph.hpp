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
    std::vector<DependencyNode> siblings;
    DependencyNode *parent;
    Filepath path;
};

#endif
