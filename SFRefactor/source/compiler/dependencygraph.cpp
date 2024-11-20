#include <compiler/dependencygraph.hpp>

// --- Dependency Node Implementation ------------------------------------------

DependencyNode::
DependencyNode(Filepath path, std::shared_ptr<DependencyNode> parent)
    : path(path), parent(parent)
{

}

DependencyNode::
~DependencyNode()
{

}

std::shared_ptr<DependencyNode> DependencyNode::
get_parent() const
{
    
    return this->parent;

}

Filepath DependencyNode::
get_path() const
{
    
    return path;

}

std::string DependencyNode::
get_path_as_string() const
{
    
    return path.c_str();

}

// --- Dependency Graph Implementation -----------------------------------------

DependencyGraph::
DependencyGraph()
{

}

DependencyGraph::
~DependencyGraph()
{

}

std::shared_ptr<DependencyNode> DependencyGraph::
get_base_node() const
{
    
    return this->base_node;

}
