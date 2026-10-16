
#ifndef BST_H
#define BST_H

#include "patient.h"

typedef struct BSTNode {
    Patient patient;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

BSTNode *insertBST(BSTNode *root, Patient patient);
Patient *searchBST(BSTNode *root, int patient_id);
void inorderBST(BSTNode *root);
void freeBST(BSTNode *root);

#endif
