#ifndef PATIENT_QUEUE_H
#define PATIENT_QUEUE_H

#include "patient.h"

typedef struct QNode {
    Patient data;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *front;
    QNode *rear;
    int size;
} Queue;

void q_init(Queue *q);
int q_enqueue(Queue *q, Patient p);
int q_dequeue(Queue *q, Patient *out);
int q_is_empty(const Queue *q);
void q_display(const Queue *q);
void q_free(Queue *q);

#endif