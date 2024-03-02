//
// Created by os on 7/31/23.
//

#ifndef OS1_PROJECT_MAIN_LIST_H
#define OS1_PROJECT_MAIN_LIST_H
struct list_t
{
    struct list_t *next = nullptr;
    struct list_t *prev = nullptr;
};

void list_push(struct list_t **list,struct list_t *entry);

void list_remove(struct list_t **list, struct list_t *entry);

struct list_t *list_pop(struct list_t **list);

#endif //OS1_PROJECT_MAIN_LIST_H
