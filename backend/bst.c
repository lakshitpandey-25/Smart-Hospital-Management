
#include <stdio.h>
#include <stdlib.h>
#include "../include/bst.h"

BSTNode *insertBST(BSTNode *root, Patient patient)
{
    if (root == NULL) {
        BSTNode *newNode = malloc(sizeof(BSTNode));

        if (newNode == NULL) {
            printf("Memory allocation failed.\n");
            return NULL;
        }

        newNode->patient = patient;
        newNode->left = NULL;
        newNode->right = NULL;

        return newNode;
    }

    if (patient.patient_id < root->patient.patient_id) {
        root->left = insertBST(root->left, patient);
    }
    else if (patient.patient_id > root->patient.patient_id) {
        root->right = insertBST(root->right, patient);
    }
    else {
        printf("Duplicate patient ID: %d\n", patient.patient_id);
    }

    return root;
}

Patient *searchBST(BSTNode *root, int patient_id)
{
    if (root == NULL) {
        return NULL;
    }

    if (root->patient.patient_id == patient_id) {
        return &root->patient;
    }

    if (patient_id < root->patient.patient_id) {
        return searchBST(root->left, patient_id);
    }

    return searchBST(root->right, patient_id);
}

void inorderBST(BSTNode *root)
{
    if (root == NULL) {
        return;
    }

    inorderBST(root->left);

    printf("ID: %d | Name: %s | Department: %s\n",
           root->patient.patient_id,
           root->patient.name,
           root->patient.department);

    inorderBST(root->right);
}

void freeBST(BSTNode *root)
{
    if (root == NULL) {
        return;
    }

    freeBST(root->left);
    freeBST(root->right);
    free(root);
}
