
#include <stdio.h>
#include "../include/resource_manager.h"

int main(void)
{
    ResourceManager manager;

    initResources(&manager, 3);

    printf("=== Initial Status ===\n");
    displayResources(&manager);

    printf("\n=== Allocate Beds ===\n");

    printf("Allocation 1: %s\n",
           allocateBed(&manager) ? "Success" : "Failed");

    printf("Allocation 2: %s\n",
           allocateBed(&manager) ? "Success" : "Failed");

    displayResources(&manager);

    printf("\n=== Release Bed ===\n");

    printf("Release: %s\n",
           releaseBed(&manager) ? "Success" : "Failed");

    displayResources(&manager);

    printf("\n=== Test Invalid Operations ===\n");

    initResources(&manager, 0);

    printf("Allocation with no beds: %s\n",
           allocateBed(&manager) ? "Success" : "Failed");

    printf("Release with no occupied beds: %s\n",
           releaseBed(&manager) ? "Success" : "Failed");

    return 0;
}
