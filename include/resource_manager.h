
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

typedef struct {
    int total_beds;
    int available_beds;
} ResourceManager;

void initResources(ResourceManager *manager, int total_beds);

int allocateBed(ResourceManager *manager);

int releaseBed(ResourceManager *manager);

void displayResources(const ResourceManager *manager);

#endif
