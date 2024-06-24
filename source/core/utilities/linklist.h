#ifndef SIGMAFOX_CORE_UTILITIES_LINKLIST_H
#define SIGMAFOX_CORE_UTILITIES_LINKLIST_H
#include <core/definitions.h>
#include <core/utilities/allocator.h>

struct snode
{
    snode   *next;
    void    *data;
};

struct singly_linked_list
{
    snode *root;
    snode *last;
};

inline snode* 
single_linked_list_append_node(singly_linked_list *list, memory_arena *arena)
{
    
}

#endif
