#include <stdio.h>
#include <string.h>
#include "action_stack.h"

static int fails = 0;

static void check(const char *name, int ok)
{
    printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) fails++;
}

int main(void)
{
    ActionStack s;
    ActionEntry e;
    st_init(&s);

    printf("--- empty stack ---\n");
    st_display(&s);
    check("new stack is empty", st_is_empty(&s) == 1);
    check("pop on empty returns -1 (underflow)", st_pop(&s, &e) == -1);

    printf("--- push ---\n");
    check("push Registered 101", st_push(&s, "Registered", 101) == 0);
    check("push Added to queue 101", st_push(&s, "Added to queue", 101) == 0);
    check("push Processed 101", st_push(&s, "Processed", 101) == 0);
    check("stack not empty", st_is_empty(&s) == 0);
    st_display(&s);

    printf("--- pop (LIFO order) ---\n");
    check("1st pop is Processed", st_pop(&s, &e) == 0 && strcmp(e.action, "Processed") == 0 && e.patient_id == 101);
    check("2nd pop is Added to queue", st_pop(&s, &e) == 0 && strcmp(e.action, "Added to queue") == 0);
    check("3rd pop is Registered", st_pop(&s, &e) == 0 && strcmp(e.action, "Registered") == 0);
    check("empty again", st_is_empty(&s) == 1);
    check("pop after empty returns -1", st_pop(&s, &e) == -1);

    printf("--- overflow ---\n");
    int ok = 1;
    for (int i = 0; i < STACK_MAX; i++)
        if (st_push(&s, "Fill", i) != 0) ok = 0;
    check("filled to STACK_MAX", ok && st_is_full(&s) == 1);
    check("push on full returns -1 (overflow)", st_push(&s, "Extra", 999) == -1);
    check("top entry unchanged after overflow", st_pop(&s, &e) == 0 && e.patient_id == STACK_MAX - 1);

    printf("--- long action text is cut safely ---\n");
    ActionStack t;
    st_init(&t);
    char longtext[200];
    memset(longtext, 'A', sizeof(longtext) - 1);
    longtext[sizeof(longtext) - 1] = '\0';
    check("push long text ok", st_push(&t, longtext, 1) == 0);
    check("stored text is cut to fit", st_pop(&t, &e) == 0 && strlen(e.action) == ACTION_LEN - 1);

    printf("\n%s (%d failed)\n", fails == 0 ? "ALL PASSED" : "SOME FAILED", fails);
    return fails;
}