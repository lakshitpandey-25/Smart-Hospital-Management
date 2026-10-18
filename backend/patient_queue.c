#include <stdio.h>
#include <stdlib.h>
#include "patient_queue.h"

void q_init(Queue *q)
{
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

int q_is_empty(const Queue *q)
{
    return q->front == NULL;
}

/* 0 = ok, -2 = memory error */
int q_enqueue(Queue *q, Patient p)
{
    QNode *n = (QNode *)malloc(sizeof(QNode));
    if (n == NULL)
        return -2;
    n->data = p;
    n->next = NULL;

    if (q->rear == NULL) {
        q->front = n;
        q->rear = n;
    } else {
        q->rear->next = n;
        q->rear = n;
    }
    q->size++;
    return 0;
}

/* 0 = ok (patient copied to *out), -1 = queue empty */
int q_dequeue(Queue *q, Patient *out)
{
    if (q->front == NULL)
        return -1;

    QNode *n = q->front;
    *out = n->data;
    q->front = n->next;
    if (q->front == NULL)
        q->rear = NULL;
    free(n);
    q->size--;
    return 0;
}

void q_display(const Queue *q)
{
    if (q->front == NULL) {
        printf("Waiting queue is empty.\n");
        return;
    }
    printf("%-6s %-20s %-4s %-8s %s\n", "ID", "Name", "Age", "Severity", "Department");
    for (QNode *cur = q->front; cur != NULL; cur = cur->next) {
        printf("%-6d %-20s %-4d %-8d %s\n",
               cur->data.patient_id, cur->data.name, cur->data.age,
               cur->data.severity, cur->data.department);
    }
}

void q_free(Queue *q)
{
    Patient tmp;
    while (q_dequeue(q, &tmp) == 0)
        ;
}