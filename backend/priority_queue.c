#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"

void pq_init(PriorityQueue *pq)
{
    pq->head = NULL;
    pq->size = 0;
}

int pq_is_empty(const PriorityQueue *pq)
{
    return pq->head == NULL;
}

/* 0 = ok, -2 = memory error, -3 = severity not in 1..5 */
int pq_insert(PriorityQueue *pq, Patient p)
{
    if (p.severity < 1 || p.severity > 5)
        return -3;

    PQNode *n = (PQNode *)malloc(sizeof(PQNode));
    if (n == NULL)
        return -2;
    n->data = p;
    n->next = NULL;

    /* go past every node with severity <= new one, so equal severity keeps arrival order */
    if (pq->head == NULL || pq->head->data.severity > p.severity) {
        n->next = pq->head;
        pq->head = n;
    } else {
        PQNode *cur = pq->head;
        while (cur->next != NULL && cur->next->data.severity <= p.severity)
            cur = cur->next;
        n->next = cur->next;
        cur->next = n;
    }
    pq->size++;
    return 0;
}

/* 0 = ok (patient copied to *out), -1 = empty */
int pq_remove_highest(PriorityQueue *pq, Patient *out)
{
    if (pq->head == NULL)
        return -1;

    PQNode *n = pq->head;
    *out = n->data;
    pq->head = n->next;
    free(n);
    pq->size--;
    return 0;
}

void pq_display(const PriorityQueue *pq)
{
    if (pq->head == NULL) {
        printf("No emergency patients.\n");
        return;
    }
    printf("%-6s %-20s %-4s %-8s %s\n", "ID", "Name", "Age", "Severity", "Department");
    for (PQNode *cur = pq->head; cur != NULL; cur = cur->next) {
        printf("%-6d %-20s %-4d %-8d %s\n",
               cur->data.patient_id, cur->data.name, cur->data.age,
               cur->data.severity, cur->data.department);
    }
}

void pq_free(PriorityQueue *pq)
{
    Patient tmp;
    while (pq_remove_highest(pq, &tmp) == 0)
        ;
}