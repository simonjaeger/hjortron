#include "list.h"
#include "memory/malloc.h"
#include "assert.h"

list_t *list_create(void)
{
    list_t *list = (list_t *)malloc(sizeof(list_t));
    assert(list);
    return list;
}

void list_destroy(list_t *list)
{
    assert(list);

    node_t *node = list->head;
    while (node != NULL)
    {
        node_t *next = node->next;
        free(node);
        node = next;
    }

    free(list);
}

node_t *list_find(list_t *list, void *value)
{
    assert(list);
    assert(value);

    node_t *node = list->head;
    while (node != NULL)
    {
        if (node->value == value)
        {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void list_insert(list_t *list, void *value)
{
    assert(list);
    assert(value);

    node_t *new = (node_t *)malloc(sizeof(node_t));
    assert(new);

    new->previous = NULL;
    new->next = NULL;
    new->value = value;

    if (list->head == NULL)
    {
        list->head = new;
        list->length++;
        return;
    }

    node_t *node = list->head;
    while (node->next != NULL)
    {
        node = node->next;
    }

    node->next = new;
    new->previous = node;
    list->length++;
}

void list_delete(list_t *list, void *value)
{
    assert(list);
    assert(value);

    node_t *node = list_find(list, value);
    assert(node);

    node_t *previous = node->previous;
    node_t *next = node->next;
    previous->next = next;
    next->previous = previous;

    if (list->head == node)
    {
        list->head = next;
    }

    list->length--;
    free(node);
}