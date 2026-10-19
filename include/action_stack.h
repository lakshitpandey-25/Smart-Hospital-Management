#ifndef ACTION_STACK_H
#define ACTION_STACK_H

#define STACK_MAX 50
#define ACTION_LEN 64

typedef struct {
    char action[ACTION_LEN];
    int patient_id;
} ActionEntry;

typedef struct {
    ActionEntry items[STACK_MAX];
    int top;
} ActionStack;

void st_init(ActionStack *s);
int st_push(ActionStack *s, const char *action, int patient_id);
int st_pop(ActionStack *s, ActionEntry *out);
int st_is_empty(const ActionStack *s);
int st_is_full(const ActionStack *s);
void st_display(const ActionStack *s);

#endif