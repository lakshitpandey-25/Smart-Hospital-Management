#include <stdio.h>
#include <string.h>
#include "linked_list.h"

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
    Node *head = NULL;

    printf("--- empty list ---\n");
    ll_display(head);
    check("delete on empty list returns -1", ll_delete(&head, 1) == -1);
    check("search on empty list returns NULL", ll_search(head, 1) == NULL);

    printf("--- insert ---\n");
    check("insert id 101", ll_insert(&head, make(101, "Asha Rao", 30, 3, "General")) == 0);
    check("insert id 102", ll_insert(&head, make(102, "Ravi Kumar", 45, 1, "Cardiology")) == 0);
    check("insert id 103", ll_insert(&head, make(103, "Meena Singh", 22, 5, "ENT")) == 0);
    check("duplicate id 102 rejected", ll_insert(&head, make(102, "Dup Name", 50, 2, "X")) == -1);
    ll_display(head);

    printf("--- search ---\n");
    Node *f = ll_search(head, 102);
    check("search existing id 102", f != NULL && strcmp(f->data.name, "Ravi Kumar") == 0);
    check("search missing id 999 returns NULL", ll_search(head, 999) == NULL);

    printf("--- delete ---\n");
    check("delete middle (102)", ll_delete(&head, 102) == 0);
    check("delete head (101)", ll_delete(&head, 101) == 0);
    check("delete last (103)", ll_delete(&head, 103) == 0);
    check("list empty after deleting all", head == NULL);
    check("delete missing id returns -1", ll_delete(&head, 555) == -1);

    ll_free(&head);
    printf("\n%s (%d failed)\n", fails == 0 ? "ALL PASSED" : "SOME FAILED", fails);
    return fails;
}