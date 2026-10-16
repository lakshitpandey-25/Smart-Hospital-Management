
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "patient.h"

#define HASH_TABLE_SIZE 101

typedef struct {
    Patient patients[HASH_TABLE_SIZE];
    int occupied[HASH_TABLE_SIZE];
} HashTable;

void initHashTable(HashTable *table);
int insertPatient(HashTable *table, Patient patient);
Patient *searchPatient(HashTable *table, int patient_id);
void displayHashTable(const HashTable *table);

#endif
