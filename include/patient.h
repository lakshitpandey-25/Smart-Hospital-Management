
#ifndef PATIENT_H
#define PATIENT_H

typedef struct {
    int patient_id;
    char name[50];
    int age;
    int severity;
    char department[30];
} Patient;

#endif
