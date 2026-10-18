#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "patient.h"

typedef struct PQNode {
    Patient data;
    struct PQNode *next;
} PQNode;

typedef struct {
    PQNode *head;
    int size;
} PriorityQueue;

void pq_init(PriorityQueue *pq);
int pq_insert(PriorityQueue *pq, Patient p);
int pq_remove_highest(PriorityQueue *pq, Patient *out);
int pq_is_empty(const PriorityQueue *pq);
void pq_display(const PriorityQueue *pq);
void pq_free(PriorityQueue *pq);

#endif