
#include <stdio.h>
#include "../include/resource_manager.h"

void initResources(ResourceManager *manager, int total_beds)
{
    if (manager == NULL) {
        return;
    }

    if (total_beds < 0) {
        total_beds = 0;
    }

    manager->total_beds = total_beds;
    manager->available_beds = total_beds;
}

int allocateBed(ResourceManager *manager)
{
    if (manager == NULL || manager->available_beds <= 0) {
        return 0;
    }

    manager->available_beds--;
    return 1;
}

int releaseBed(ResourceManager *manager)
{
    if (manager == NULL ||
        manager->available_beds >= manager->total_beds) {
        return 0;
    }

    manager->available_beds++;
    return 1;
}

void displayResources(const ResourceManager *manager)
{
    if (manager == NULL) {
        printf("Invalid resource manager.\n");
        return;
    }

    printf("\n--- Hospital Resource Status ---\n");
    printf("Total Beds: %d\n", manager->total_beds);
    printf("Available Beds: %d\n", manager->available_beds);
    printf("Occupied Beds: %d\n",
           manager->total_beds - manager->available_beds);
}
