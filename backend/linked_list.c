#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

Node *ll_search(Node *head, int patient_id)
{
    Node *cur = head;
    while (cur != NULL) {
        if (cur->data.patient_id == patient_id)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

/* 0 = ok, -1 = duplicate ID, -2 = memory error */
int ll_insert(Node **head, Patient p)
{
    if (ll_search(*head, p.patient_id) != NULL)
        return -1;

    Node *n = (Node *)malloc(sizeof(Node));
    if (n == NULL)
        return -2;
    n->data = p;
    n->next = NULL;

    if (*head == NULL) {
        *head = n;
        return 0;
    }
    Node *cur = *head;
    while (cur->next != NULL)
        cur = cur->next;
    cur->next = n;
    return 0;
}

/* 0 = deleted, -1 = ID not found (or list empty) */
int ll_delete(Node **head, int patient_id)
{
    Node *cur = *head;
    Node *prev = NULL;

    while (cur != NULL && cur->data.patient_id != patient_id) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL)
        return -1;

    if (prev == NULL)
        *head = cur->next;
    else
        prev->next = cur->next;
    free(cur);
    return 0;
}

void ll_display(Node *head)
{
    if (head == NULL) {
        printf("No patient records.\n");
        return;
    }
    printf("%-6s %-20s %-4s %-8s %s\n", "ID", "Name", "Age", "Severity", "Department");
    for (Node *cur = head; cur != NULL; cur = cur->next) {
        printf("%-6d %-20s %-4d %-8d %s\n",
               cur->data.patient_id, cur->data.name, cur->data.age,
               cur->data.severity, cur->data.department);
    }
}

void ll_free(Node **head)
{
    Node *cur = *head;
    while (cur != NULL) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    *head = NULL;
}