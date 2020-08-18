#ifndef LIST_H_
#define LIST_H_

#include <stddef.h>

typedef struct node
{
    struct node *previous;
    struct node *next;
    void *value;
} node_t;

typedef struct list
{
    node_t *head;
    size_t length;
} list_t;

list_t *list_create(void);
void list_destroy(list_t *list);

node_t *list_find(list_t *list, void *value);
void list_insert(list_t *list, void *value);
void list_delete(list_t *list, void *value);

#endif // LIST_H_