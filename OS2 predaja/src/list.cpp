//
// Created by os on 7/31/23.
//

#include "../h/list.h"

void list_push(struct list_t **list, struct list_t *entry)
{
    if (*list == nullptr)
    {
        *list = entry;
        entry->next = nullptr;
        entry->prev = nullptr;
    }
    else
    {
        entry->next = *list;
        entry->prev = nullptr;
        (*list)->prev = entry;
        *list = entry;
    }
}

void list_remove(struct list_t **list, struct list_t *entry)
{
    if (*list == nullptr || entry == nullptr)
        return;

    if (*list == entry)
        *list = entry->next;

    if (entry->prev != nullptr)
        entry->prev->next = entry->next;

    if (entry->next != nullptr)
        entry->next->prev = entry->prev;

    entry->next = nullptr;
    entry->prev = nullptr;
}

struct list_t *list_pop(struct list_t **list)
{
    if (*list == nullptr)
        return nullptr;

    struct list_t *popEntry = *list;
    *list = (*list)->next;

    if (*list != nullptr)
        (*list)->prev = nullptr;

    popEntry->next = nullptr;
    popEntry->prev = nullptr;

    return popEntry;
}
