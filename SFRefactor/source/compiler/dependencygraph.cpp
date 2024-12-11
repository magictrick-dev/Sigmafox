#include <iostream>
#include <compiler/parser.hpp>
#include <compiler/dependencygraph.hpp>
#include <compiler/errorhandler.hpp>

// --- Dependency Node ---------------------------------------------------------

DependencyNode::
DependencyNode(shared_ptr<DependencyNode> parent, Filepath path, DependencyGraph* graph)
{

    this->path = path;
    this->parent = parent;
    this->parser = std::make_shared<SyntaxParser>(path, graph);

}

DependencyNode::
~DependencyNode()
{

}

Filepath DependencyNode::
get_path() const
{

    return this->path;

}

shared_ptr<SyntaxParser> DependencyNode::
get_parser() const
{

    return this->parser;

}

shared_ptr<DependencyNode> DependencyNode::
get_parent_node() const
{

    return this->parent;

}

bool DependencyNode::
add_child(shared_ptr<DependencyNode> child)
{

    // NOTE(Chris): This is a somewhat expensive operation if we were to have a lot
    //              includes in a single file, but generally there isn't going to be.
    for (auto current_child : this->children)
    {
        if (current_child->get_path() == child->get_path())
        {
            std::cout << "Attempting to include a file that is already included.";
            return false;
        }
    }

    this->children.push_back(child);
    return true;

}

// --- Dependency Graph --------------------------------------------------------

DependencyGraph::
DependencyGraph()
{

}

DependencyGraph::
~DependencyGraph()
{

}

bool DependencyGraph::
set_entry(Filepath entry)
{

    if (!entry.is_valid_file()) return false;
    auto node = std::make_shared<DependencyNode>(nullptr, entry, this);
    this->entry_node = node;
    this->node_map[entry.c_str()] = node;
    this->node_list.push_back(node);
    return true;

}

bool DependencyGraph::
insert_dependency(Filepath parent, Filepath child)
{

    // Parent must exist, otherwise something wack happened.
    SF_ASSERT(this->node_map.find(parent.c_str()) != this->node_map.end());
    shared_ptr<DependencyNode> parent_node = this->node_map[parent.c_str()];
    
    // Search for circular dependency. Essentially we just traverse up to the
    // entry file and check if the name of the child matches any of them.
    auto search = parent_node;
    std::string proposed_path = child.c_str();
    while (search != nullptr)
    {

        std::string parent_path = search->get_path().c_str();
        if (parent_path == proposed_path)
        {
            std::cout   << "Circular inclusion found in " << parent_path << " for "
                        << child << "." << std::endl;
            return false;
        }

        search = parent_node->get_parent_node();

    }

    // NOTE(Chris): Since nodes are what maintain the parsers, we need to ensure
    //              that we only have one actual node and pass it around when it has
    //              multiple parents. We essentially find it, if it isn't there create
    //              it, otherwise get it and add it to the list of children.
    auto potential_child = this->node_map.find(child.c_str());
    shared_ptr<DependencyNode> child_node = nullptr;
    if (potential_child == this->node_map.end())
    {

        child_node = std::make_shared<DependencyNode>(parent_node, child, this);
        this->node_map[child.c_str()] = child_node;
        this->node_list.push_back(child_node);

    }
    else
    {
        child_node = potential_child->second;
    }

    // Add that node to the parent's children.
    parent_node->add_child(child_node);
    return true;

}

shared_ptr<SyntaxParser> DependencyGraph::
get_parser_for(Filepath path)
{

    auto search = this->node_map.find(path.c_str());
    SF_ASSERT(search != this->node_map.end());
    return search->second->get_parser();

}
