#include <stdio.h>
#include <string.h>
#include "file_handler.h"

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
    const char *path = "test_patients.txt";
    Node *a = NULL, *b = NULL;
    int skipped = 0;

    printf("--- save empty list ---\n");
    check("save empty list returns 0", fh_save(path, a) == 0);
    check("load empty file returns 0", fh_load(path, &b, &skipped) == 0 && skipped == 0);

    printf("--- save and load ---\n");
    ll_insert(&a, make(101, "Asha Rao", 30, 3, "General"));
    ll_insert(&a, make(102, "Ravi Kumar", 45, 1, "Cardiology"));
    ll_insert(&a, make(103, "Meena Singh", 22, 5, "ENT"));
    check("save 3 records returns 3", fh_save(path, a) == 3);

    check("load 3 records returns 3", fh_load(path, &b, &skipped) == 3 && skipped == 0);
    Node *f = ll_search(b, 102);
    check("loaded id 102 has same data", f != NULL && strcmp(f->data.name, "Ravi Kumar") == 0
          && f->data.age == 45 && f->data.severity == 1 && strcmp(f->data.department, "Cardiology") == 0);
    check("order preserved (head is 101)", b != NULL && b->data.patient_id == 101);
    ll_display(b);

    printf("--- missing and invalid file ---\n");
    Node *c = NULL;
    check("load missing file returns -1", fh_load("no_such_file.txt", &c, &skipped) == -1);
    check("save to invalid path returns -1", fh_save("no_such_folder/x.txt", a) == -1);

    printf("--- bad lines are skipped ---\n");
    FILE *w = fopen(path, "w");
    fputs("401|Good One|40|2|General\n", w);
    fputs("this is not a record\n", w);
    fputs("402|Bad Sev|30|9|ENT\n", w);
    fputs("403|No Dept|30|2\n", w);
    fputs("401|Duplicate|50|3|X\n", w);
    fputs("404|Also Good|22|5|ENT\n", w);
    fclose(w);

    Node *d = NULL;
    int loaded = fh_load(path, &d, &skipped);
    check("2 good lines loaded", loaded == 2);
    check("4 bad lines skipped", skipped == 4);
    check("401 and 404 present", ll_search(d, 401) != NULL && ll_search(d, 404) != NULL);
    check("402 (bad severity) not loaded", ll_search(d, 402) == NULL);

    ll_free(&a);
    ll_free(&b);
    ll_free(&c);
    ll_free(&d);
    remove(path);

    printf("\n%s (%d failed)\n", fails == 0 ? "ALL PASSED" : "SOME FAILED", fails);
    return fails;
}