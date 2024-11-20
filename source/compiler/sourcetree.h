#ifndef SOURCE_COMPILER_SOURCETREE_H
#define SOURCE_COMPILER_SOURCETREE_H
#include <core/definitions.h>

/*
typedef struct source_file_node
{
    
    cc64 path;
    source_file_node *parent;
    source_file_node *dependency_head;
    source_file_node *dependency_tail;
    source_file_node *next_sibling;

} source_file_node;
*/

/*

source_file_node *main;
main->parent = NULL;
source_parser_match_imports(parser);

..match_imports(parser)
{

    ccptr depenencies[n]; ...
    source_file_node *nodes[n]; ...

    syntax_node *head_module_node;
    syntax_node *tail_module_node;
    for (path : dependencies, nodes)
    {

        self_node->path = path;
        syntax_node *module_node = match_submodule(parser, self_node);
        
        // update head & tail module nodes

    }

}

*/

#endif
