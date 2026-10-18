#include <stdio.h>
#include <string.h>
#include "file_handler.h"

static void clean_text(char *s)
{
    for (; *s != '\0'; s++) {
        if (*s == '|' || *s == '\n' || *s == '\r')
            *s = ' ';
    }
}

int fh_save(const char *path, const Node *head)
{
    FILE *f = fopen(path, "w");
    if (f == NULL)
        return -1;

    int count = 0;
    for (const Node *cur = head; cur != NULL; cur = cur->next) {
        char name[50], dept[30];
        strncpy(name, cur->data.name, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        strncpy(dept, cur->data.department, sizeof(dept) - 1);
        dept[sizeof(dept) - 1] = '\0';
        clean_text(name);
        clean_text(dept);

        fprintf(f, "%d|%s|%d|%d|%s\n",
                cur->data.patient_id, name, cur->data.age,
                cur->data.severity, dept);
        count++;
    }

    if (fclose(f) != 0)
        return -1;
    return count;
}

int fh_load(const char *path, Node **head, int *skipped)
{
    if (skipped != NULL)
        *skipped = 0;

    FILE *f = fopen(path, "r");
    if (f == NULL)
        return -1;

    char line[256];
    int loaded = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        if (line[0] == '\n' || line[0] == '\r')
            continue; /* blank line, ignore */

        Patient p;
        char name[50], dept[30];
        memset(&p, 0, sizeof(p));

        int n = sscanf(line, "%d|%49[^|]|%d|%d|%29[^|\r\n]",
                       &p.patient_id, name, &p.age, &p.severity, dept);

        if (n != 5 || p.patient_id <= 0 || p.age < 0 || p.age > 150 ||
            p.severity < 1 || p.severity > 5) {
            if (skipped != NULL) (*skipped)++;
            continue;
        }

        strcpy(p.name, name);
        strcpy(p.department, dept);

        if (ll_insert(head, p) != 0) { /* duplicate id or memory error */
            if (skipped != NULL) (*skipped)++;
            continue;
        }
        loaded++;
    }

    fclose(f);
    return loaded;
}