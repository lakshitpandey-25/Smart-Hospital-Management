#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "patient.h"

typedef struct Node {
    Patient data;
    struct Node *next;
} Node;

int ll_insert(Node **head, Patient p);
int ll_delete(Node **head, int patient_id);
Node *ll_search(Node *head, int patient_id);
void ll_display(Node *head);
void ll_free(Node **head);

#endif