#include <stdio.h>
#include <string.h>
#include "priority_queue.h"

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
    PriorityQueue pq;
    Patient out;
    pq_init(&pq);

    printf("--- empty ---\n");
    pq_display(&pq);
    check("new pq is empty", pq_is_empty(&pq) == 1);
    check("remove on empty returns -1", pq_remove_highest(&pq, &out) == -1);

    printf("--- invalid severity ---\n");
    check("severity 0 rejected", pq_insert(&pq, make(900, "Bad Zero", 20, 0, "X")) == -3);
    check("severity 6 rejected", pq_insert(&pq, make(901, "Bad Six", 20, 6, "X")) == -3);
    check("still empty after rejects", pq_is_empty(&pq) == 1);

    printf("--- insert out of order ---\n");
    check("insert 301 sev3", pq_insert(&pq, make(301, "Asha Rao", 30, 3, "General")) == 0);
    check("insert 302 sev1", pq_insert(&pq, make(302, "Ravi Kumar", 45, 1, "Cardiology")) == 0);
    check("insert 303 sev5", pq_insert(&pq, make(303, "Meena Singh", 22, 5, "ENT")) == 0);
    check("insert 304 sev1", pq_insert(&pq, make(304, "Kiran Das", 60, 1, "Emergency")) == 0);
    check("insert 305 sev2", pq_insert(&pq, make(305, "Sita Devi", 35, 2, "Neurology")) == 0);
    check("size is 5", pq.size == 5);
    pq_display(&pq);

    printf("--- remove order: sev 1 first, same sev by arrival ---\n");
    check("1st is 302 (sev1, came first)", pq_remove_highest(&pq, &out) == 0 && out.patient_id == 302);
    check("2nd is 304 (sev1, came later)", pq_remove_highest(&pq, &out) == 0 && out.patient_id == 304);
    check("3rd is 305 (sev2)", pq_remove_highest(&pq, &out) == 0 && out.patient_id == 305);
    check("4th is 301 (sev3)", pq_remove_highest(&pq, &out) == 0 && out.patient_id == 301);
    check("5th is 303 (sev5)", pq_remove_highest(&pq, &out) == 0 && out.patient_id == 303);
    check("empty again", pq_is_empty(&pq) == 1 && pq.size == 0);
    check("remove after empty returns -1", pq_remove_highest(&pq, &out) == -1);

    printf("--- reuse after empty ---\n");
    check("insert 306 sev4", pq_insert(&pq, make(306, "Test Six", 40, 4, "General")) == 0);
    check("remove gives 306", pq_remove_highest(&pq, &out) == 0 && out.patient_id == 306);

    pq_free(&pq);
    printf("\n%s (%d failed)\n", fails == 0 ? "ALL PASSED" : "SOME FAILED", fails);
    return fails;
}