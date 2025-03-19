#include <compiler/graph.hpp>

DependencyGraph::
DependencyGraph()
{
    
    this->root = nullptr;
    
}

DependencyGraph::
~DependencyGraph()
{

}

void DependencyGraph::
set_root(string path)
{
    
    if (this->root == nullptr)
    {
        
        this->root = make_shared<DependencyNode>();
        this->root->path = path;

        this->nodes[path] = this->root;

    }

}

string DependencyGraph::
get_root_path() const
{
    
    if (this->root == nullptr)
        return "";
    
    return this->root->path;
    
}

DependencyResult DependencyGraph::
add_dependency(string parent, string child)
{
    
    // Find the parent.
    if (this->nodes.find(parent) == this->nodes.end())
        return DependencyResult::DEPENDENCY_PARENT_NOT_FOUND;
    
    // Fetch parent, if parent is the child, return false.
    auto parent_node = this->nodes[parent];
    if (parent_node->path == child)
        return DependencyResult::DEPENDENCY_SELF_INCLUDED;
    
    // Check if the child already exists.
    if (this->has_dependency(parent, child))
        return DependencyResult::DEPENDENCY_ALREADY_INCLUDED;

    // Ensure that the child isn't circularly included.
    auto search = parent_node;
    while (search != nullptr)
    {
        
        if (search->path == child)
        {
            return DependencyResult::DEPENDENCY_IS_CIRCULAR;
        }
        
        search = search->parent;
        
    }

    // Create the child node if it doesn't exist.
    shared_ptr<DependencyNode> child_node = nullptr;
    if (this->nodes.find(child) == this->nodes.end())
    {
        
        child_node = make_shared<DependencyNode>();
        child_node->path = child;
        child_node->parent = parent_node;
        
        this->nodes[child] = child_node;
        
    }
    else
    {
        child_node = this->nodes[child];
    }

    return DependencyResult::DEPENDENCY_SUCCESS;
    
}

bool DependencyGraph::
has_dependency(string parent, string child)
{
    
    if (this->nodes.find(parent) == this->nodes.end())
        return false;
    
    auto parent_node = this->nodes[parent];
    for (auto current_child : parent_node->children)
    {
        if (current_child->path == child)
        {
            return true;
        }
    }
    
    return false;
    
}
