#include <compiler/dependencyresolver.hpp>

DependencyResolver::
DependencyResolver(SyntaxParser *entry)
{

    // Set our entry parser.
    this->entry = entry;

    // The first node is our entry node.
    this->graph = std::make_shared<DependencyNode>(entry->get_source_path(), nullptr);
    this->parsers.push_back(entry);

    return;

}

DependencyResolver::
~DependencyResolver()
{

    return;

}

bool DependencyResolver::
resolve_recurse(std::shared_ptr<DependencyNode> current, SyntaxParser *parser,
        std::vector<SyntaxParser*> &parsers)
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
            if (check_node->get_path() == current_path)
            {
                
                std::cout << "[ Parser ] Circular inclusion detected: " << current_path << std::endl;
                return false;

            }
            
            check_node = check_node->get_parent();

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
        std::shared_ptr<DependencyNode> current_node = 
            std::make_shared<DependencyNode>(current_path, current);
        current->siblings.push_back(current_node);
       
        bool result = resolve_recurse(current_node, current_parser, parsers);
        if (!result) return false; // If the recursion is broken, then we break.

    }

    return true;

}

bool DependencyResolver::
resolve()
{

    bool result = this->resolve_recurse(this->graph, this->entry, this->parsers);
    return result;

}

std::vector<SyntaxParser*> DependencyResolver::
get_dependent_parsers() const
{

    return this->parsers;

}

std::shared_ptr<DependencyNode> DependencyResolver::
get_dependency_graph() const
{

    return this->graph;

}

