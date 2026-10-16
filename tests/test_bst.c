
#include <stdio.h>
#include "../include/bst.h"

int main(void)
{
    BSTNode *root = NULL;

    Patient p1 = {1001, "Rahul", 20, 2, "Cardiology"};
    Patient p2 = {1002, "Aman", 25, 1, "Emergency"};
    Patient p3 = {999, "Priya", 30, 3, "Neurology"};

    root = insertBST(root, p1);
    root = insertBST(root, p2);
    root = insertBST(root, p3);

    printf("=== Patients in Ascending ID Order ===\n");
    inorderBST(root);

    printf("\n=== Search Patient ===\n");

    Patient *found = searchBST(root, 1002);

    if (found != NULL) {
        printf("Patient Found: %s\n", found->name);
        printf("Patient ID: %d\n", found->patient_id);
    } else {
        printf("Patient not found.\n");
    }

    printf("\n=== Search Missing Patient ===\n");

    found = searchBST(root, 5555);

    if (found != NULL) {
        printf("Patient Found: %s\n", found->name);
    } else {
        printf("Patient not found.\n");
    }

    freeBST(root);

    return 0;
}
