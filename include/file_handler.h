#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "linked_list.h"

/* returns number of records saved, -1 if file cannot be opened/written */
int fh_save(const char *path, const Node *head);

/* returns number of records loaded, -1 if file cannot be opened.
   *skipped = number of invalid/duplicate lines ignored */
int fh_load(const char *path, Node **head, int *skipped);

#endif