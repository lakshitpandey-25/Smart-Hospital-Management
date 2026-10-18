#include <stdio.h>
#include <string.h>
#include "action_stack.h"

void st_init(ActionStack *s)
{
    s->top = -1;
}

int st_is_empty(const ActionStack *s)
{
    return s->top == -1;
}

int st_is_full(const ActionStack *s)
{
    return s->top == STACK_MAX - 1;
}

/* 0 = ok, -1 = stack overflow (full) */
int st_push(ActionStack *s, const char *action, int patient_id)
{
    if (st_is_full(s))
        return -1;
    s->top++;
    strncpy(s->items[s->top].action, action, ACTION_LEN - 1);
    s->items[s->top].action[ACTION_LEN - 1] = '\0';
    s->items[s->top].patient_id = patient_id;
    return 0;
}

/* 0 = ok (entry copied to *out), -1 = stack underflow (empty) */
int st_pop(ActionStack *s, ActionEntry *out)
{
    if (st_is_empty(s))
        return -1;
    *out = s->items[s->top];
    s->top--;
    return 0;
}

/* shows newest action first */
void st_display(const ActionStack *s)
{
    if (st_is_empty(s)) {
        printf("No recent actions.\n");
        return;
    }
    printf("Recent actions (newest first):\n");
    for (int i = s->top; i >= 0; i--)
        printf("%d. %s (patient %d)\n", s->top - i + 1,
               s->items[i].action, s->items[i].patient_id);
}