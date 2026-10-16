
#include "../include/search_sort.h"

/* Search patient ID one by one */
int linearSearch(Patient patients[], int count, int patient_id)
{
    for (int i = 0; i < count; i++) {
        if (patients[i].patient_id == patient_id) {
            return i;
        }
    }

    return -1;
}

/* Binary search requires IDs sorted in ascending order */
int binarySearch(Patient patients[], int count, int patient_id)
{
    int low = 0;
    int high = count - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (patients[mid].patient_id == patient_id) {
            return mid;
        } else if (patients[mid].patient_id < patient_id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

/* Sort complete patient records by ID using Bubble Sort */
void sortPatientsByID(Patient patients[], int count)
{
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (patients[j].patient_id > patients[j + 1].patient_id) {
                Patient temp = patients[j];
                patients[j] = patients[j + 1];
                patients[j + 1] = temp;
            }
        }
    }
}
