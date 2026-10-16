
#include <stdio.h>
#include "../include/patient.h"

int main() {
    Patient p1 = {1001, "Rahul", 20, 2, "Cardiology"};

    printf("=================================\n");
    printf(" SMART HOSPITAL MANAGEMENT SYSTEM\n");
    printf("=================================\n");

    printf("Patient ID : %d\n", p1.patient_id);
    printf("Name       : %s\n", p1.name);
    printf("Age        : %d\n", p1.age);
    printf("Severity   : %d\n", p1.severity);
    printf("Department : %s\n", p1.department);

    return 0;
}
