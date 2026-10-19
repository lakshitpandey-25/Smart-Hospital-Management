#include <stdio.h>
#include <string.h>
#include "patient_queue.h"

static int fails = 0;

static void check(const char *name, int ok)
{
    printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) fails++;
}

static Patient make(int id, const char *name, int age, int sev, const char *dept)
{
    Patient p;
    p.patient_id = id;
    strncpy(p.name, name, sizeof(p.name) - 1);
    p.name[sizeof(p.name) - 1] = '\0';
    p.age = age;
    p.severity = sev;
    strncpy(p.department, dept, sizeof(p.department) - 1);
    p.department[sizeof(p.department) - 1] = '\0';
    return p;
}

int main(void)
{
    Queue q;
    Patient out;
    q_init(&q);

    printf("--- empty queue ---\n");
    q_display(&q);
    check("new queue is empty", q_is_empty(&q) == 1);
    check("dequeue on empty returns -1", q_dequeue(&q, &out) == -1);

    printf("--- enqueue ---\n");
    check("enqueue 201", q_enqueue(&q, make(201, "Asha Rao", 30, 3, "General")) == 0);
    check("enqueue 202", q_enqueue(&q, make(202, "Ravi Kumar", 45, 2, "Cardiology")) == 0);
    check("enqueue 203", q_enqueue(&q, make(203, "Meena Singh", 22, 5, "ENT")) == 0);
    check("size is 3", q.size == 3);
    check("queue not empty", q_is_empty(&q) == 0);
    q_display(&q);

    printf("--- dequeue (FIFO order) ---\n");
    check("1st dequeue is 201", q_dequeue(&q, &out) == 0 && out.patient_id == 201);
    check("2nd dequeue is 202", q_dequeue(&q, &out) == 0 && out.patient_id == 202);
    check("size is 1", q.size == 1);
    check("3rd dequeue is 203", q_dequeue(&q, &out) == 0 && out.patient_id == 203);
    check("queue empty again", q_is_empty(&q) == 1 && q.size == 0);
    check("dequeue after empty returns -1", q_dequeue(&q, &out) == -1);

    printf("--- reuse after empty ---\n");
    check("enqueue 204 after empty", q_enqueue(&q, make(204, "Test Four", 40, 4, "General")) == 0);
    check("dequeue gives 204", q_dequeue(&q, &out) == 0 && out.patient_id == 204);

    q_free(&q);
    printf("\n%s (%d failed)\n", fails == 0 ? "ALL PASSED" : "SOME FAILED", fails);
    return fails;
}