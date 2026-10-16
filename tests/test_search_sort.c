
#include <stdio.h>
#include "../include/search_sort.h"

int main(void)
{
    Patient patients[] = {
        {1003, "Priya", 30, 3, "Neurology"},
        {1001, "Rahul", 20, 2, "Cardiology"},
        {1002, "Aman", 25, 1, "Emergency"}
    };

    int count = sizeof(patients) / sizeof(patients[0]);

    printf("=== Linear Search ===\n");

    int index = linearSearch(patients, count, 1002);

    if (index != -1) {
        printf("Found: %s, ID: %d\n",
               patients[index].name,
               patients[index].patient_id);
    } else {
        printf("Patient not found.\n");
    }

    printf("\n=== Sorting by Patient ID ===\n");

    sortPatientsByID(patients, count);

    for (int i = 0; i < count; i++) {
        printf("%d - %s\n",
               patients[i].patient_id,
               patients[i].name);
    }

    printf("\n=== Binary Search ===\n");

    index = binarySearch(patients, count, 1003);

    if (index != -1) {
        printf("Found: %s, ID: %d\n",
               patients[index].name,
               patients[index].patient_id);
    } else {
        printf("Patient not found.\n");
    }

    return 0;
}
