#include <compiler/syntaxtree.hpp>

SyntaxTree::
SyntaxTree()
{

    this->root = nullptr;

}

SyntaxTree::
~SyntaxTree()
{

}

bool SyntaxTree::
construct_ast(Filepath entry_file)
{

    if (!this->graph.set_entry(entry_file)) return false;
    this->entry_file = entry_file;
    shared_ptr<SyntaxParser> entry_parser = this->graph.get_parser_for(entry_file);
    if (!entry_parser->construct_as_root()) return false;
    this->root = entry_parser->get_base_node();
    return true;

}

void SyntaxTree::
visit_root(ISyntaxNodeVisitor *visitor)
{

    SF_ENSURE_PTR(visitor);
    SF_ENSURE_PTR(this->root);
    this->root->accept(visitor);

}

void SyntaxTree::
dump_dependency_graph()
{

    auto dependency_list = this->graph.get_dependencies_list_recursively_for(this->entry_file);
    for (auto dependency : dependency_list)
        std::cout << dependency << std::endl;

}
