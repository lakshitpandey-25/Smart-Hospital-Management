
#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include "patient.h"

int linearSearch(Patient patients[], int count, int patient_id);

int binarySearch(Patient patients[], int count, int patient_id);

void sortPatientsByID(Patient patients[], int count);

#endif
