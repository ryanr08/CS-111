//NAME: Ryan Riahi
//EMAIL: ryanr08@gmail.com
//ID: 105138860

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "SortedList.h"

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
    if (element == NULL || list == NULL)
        return;

    if (list->next == list)
    {
        if (opt_yield & INSERT_YIELD)
            sched_yield();
        list->next = element;
        list->prev = element;
        element->next = list;
        element->prev = list;
        return;
    }

    SortedListElement_t *current = list->next;

    while (current != list)
    {
        if (strcmp(element->key, current->key) <= 0)
        {
            if (opt_yield & INSERT_YIELD)
                sched_yield();
            element->prev = current->prev;
            element->next = current;
            current->prev->next = element;
            current->prev = element;
            return;
        }
        current = current->next;
    }

    if (current == list)
    {
        if (opt_yield & INSERT_YIELD)
            sched_yield();

        element->next = list;
        element->prev = list->prev;
        list->prev->next = element;
        list->prev = element;
        return;
    }
}

int SortedList_delete(SortedListElement_t *element)
{
    if (element == NULL || element->next->prev != element || element->prev->next != element)
        return 1;
    if (opt_yield & DELETE_YIELD)
        sched_yield();
    element->prev->next = element->next;
    element->next->prev = element->prev;
    return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
    if (list == NULL || key == NULL)
        return NULL;

    for (SortedListElement_t *current = list->next; current != list; current = current->next)
    {
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        if (*current->key == *key)
            return current;
    }

    return NULL;
}

int SortedList_length(SortedList_t *list)
{
    if (list == NULL)
        return -1;
    int count = 1;
    SortedListElement_t *current = list->next;
    while (current != list)
    {
        if (current == NULL || current->next == NULL || current->prev == NULL)
        {
            //fprintf(stdout, "%c:::", *current->key);
            //fprintf(stdout, "HERE1");
            return -1;
        }
        if (current->prev->next != current || current->next->prev != current)
        {
            //fprintf(stdout, "%c:::", *current->key);
            //fprintf(stdout, "HERE2");
            return -1;
        }
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        count++;
        current = current->next;
    }

    return count;
}