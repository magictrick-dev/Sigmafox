#ifndef SIGMAFOX_CORE_UTILITIES_LINKLIST_H
#define SIGMAFOX_CORE_UTILITIES_LINKLIST_H
#include <core/definitions.h>
#include <core/utilities/allocator.h>

struct llnode
{
    llnode *next    = NULL;
    llnode *prev    = NULL;
    void   *data    = NULL;
};

struct linked_list
{
    llnode *head    = NULL;
    llnode *tail    = NULL;
    uint64_t count  = NULL;
};

inline llnode*
linked_list_push_node(linked_list *list, memory_arena *arena)
{

    llnode *instance = memory_arena_push_type(arena, llnode);

    if (list->head == NULL)
    {
        list->head = instance;   
        list->tail = instance;
    }

    else
    {
        instance->next = NULL;
        instance->prev = list->tail;
        list->tail->next = instance;
        list->tail = instance;
        list->count++;
    }

    return instance;

}

#endif
