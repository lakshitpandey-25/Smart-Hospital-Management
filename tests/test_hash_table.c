
#include <stdio.h>
#include "../include/hash_table.h"

int main(void)
{
    HashTable table;
    initHashTable(&table);
    printf("HASH_TABLE_SIZE = %d, 1001 %% HASH_TABLE_SIZE = %d\n",
       HASH_TABLE_SIZE, 1001 % HASH_TABLE_SIZE);

    Patient p1 = {1001, "Rahul", 20, 2, "Cardiology"};
    Patient p2 = {1102, "Aman", 25, 1, "Emergency"};
    Patient p3 = {1203, "Priya", 30, 3, "Neurology"};

    printf("=== Hash Table Testing ===\n");

    printf("Insert Rahul: %d\n", insertPatient(&table, p1));
    printf("Insert Aman: %d\n", insertPatient(&table, p2));
    printf("Insert Priya: %d\n", insertPatient(&table, p3));

    /* Test duplicate patient ID */
    printf("Insert duplicate ID: %d\n", insertPatient(&table, p1));

    /* Search for an existing patient */
    Patient *result = searchPatient(&table, 1102);

    if (result != NULL) {
        printf("\nPatient Found: %s\n", result->name);
        printf("Patient ID: %d\n", result->patient_id);
    } else {
        printf("\nPatient not found.\n");
    }

    /* Search for a missing patient */
    result = searchPatient(&table, 9999);
    printf("Search missing ID: %s\n",
           result == NULL ? "Not found" : "Found");

    displayHashTable(&table);

    return 0;
}
