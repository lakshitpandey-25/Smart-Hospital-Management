#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "patient.h"
#include "linked_list.h"
#include "file_handler.h"
#include "hash_table.h"
#include "patient_queue.h"
#include "priority_queue.h"
#include "resource_manager.h"

#define PATIENTS_FILE "data/patients.txt"
#define BEDS_FILE "data/bed_assignments.txt"
#define BED_CAPACITY 20

static Node *patients = NULL;
static HashTable hash_table;
static PriorityQueue emergency_queue;
static Queue normal_queue;
static ResourceManager resources;
static int bed_ids[BED_CAPACITY];
static int bed_count = 0;

static void json_string(const char *s)
{
    putchar('"');
    for (const unsigned char *p = (const unsigned char *)s; *p; ++p) {
        switch (*p) {
            case '"':  fputs("\\\"", stdout); break;
            case '\\': fputs("\\\\", stdout); break;
            case '\n': fputs("\\n", stdout); break;
            case '\r': fputs("\\r", stdout); break;
            case '\t': fputs("\\t", stdout); break;
            default:
                if (*p < 32) printf("\\u%04x", (unsigned int)*p);
                else putchar(*p);
        }
    }
    putchar('"');
}

static int patient_has_bed(int id)
{
    for (int i = 0; i < bed_count; ++i)
        if (bed_ids[i] == id) return 1;
    return 0;
}

static void print_patient(const Patient *p)
{
    printf("{\"id\":%d,\"name\":", p->patient_id);
    json_string(p->name);
    printf(",\"age\":%d,\"severity\":%d,\"department\":", p->age, p->severity);
    json_string(p->department);
    printf(",\"hasBed\":%s}", patient_has_bed(p->patient_id) ? "true" : "false");
}

static int parse_positive_int(const char *s, int *out)
{
    char *end = NULL;
    long value;
    if (s == NULL || *s == '\0') return 0;
    value = strtol(s, &end, 10);
    if (*end != '\0' || value < 1 || value > 2147483647L) return 0;
    *out = (int)value;
    return 1;
}

static int parse_int_range(const char *s, int min, int max, int *out)
{
    char *end = NULL;
    long value;
    if (s == NULL || *s == '\0') return 0;
    value = strtol(s, &end, 10);
    if (*end != '\0' || value < min || value > max) return 0;
    *out = (int)value;
    return 1;
}

static int contains_forbidden_separator(const char *s)
{
    return strchr(s, '|') != NULL || strchr(s, '\n') != NULL || strchr(s, '\r') != NULL;
}

static int load_state(void)
{
    int skipped = 0;
    int loaded = fh_load(PATIENTS_FILE, &patients, &skipped);
    (void)loaded; /* A missing file is normal on the first run. */

    initHashTable(&hash_table);
    pq_init(&emergency_queue);
    q_init(&normal_queue);
    initResources(&resources, BED_CAPACITY);

    for (Node *cur = patients; cur != NULL; cur = cur->next) {
        insertPatient(&hash_table, cur->data);
        if (cur->data.severity <= 2)
            (void)pq_insert(&emergency_queue, cur->data);
        else
            (void)q_enqueue(&normal_queue, cur->data);
    }

    FILE *f = fopen(BEDS_FILE, "r");
    if (f != NULL) {
        int id;
        while (fscanf(f, "%d", &id) == 1) {
            if (bed_count >= BED_CAPACITY) break;
            if (searchPatient(&hash_table, id) == NULL || patient_has_bed(id))
                continue;
            if (allocateBed(&resources)) bed_ids[bed_count++] = id;
        }
        fclose(f);
    }
    return 1;
}

static int save_beds(void)
{
    FILE *f = fopen(BEDS_FILE, "w");
    if (f == NULL) return 0;
    for (int i = 0; i < bed_count; ++i)
        fprintf(f, "%d\n", bed_ids[i]);
    return fclose(f) == 0;
}

static void print_resources(void)
{
    printf("{\"total\":%d,\"available\":%d,\"occupied\":%d}",
           resources.total_beds, resources.available_beds,
           resources.total_beds - resources.available_beds);
}

static void print_list(void)
{
    printf("{\"ok\":true,\"patients\":[");
    int first = 1;
    for (Node *cur = patients; cur != NULL; cur = cur->next) {
        if (!first) putchar(',');
        print_patient(&cur->data);
        first = 0;
    }

    printf("],\"normalQueue\":[");
    first = 1;
    for (QNode *cur = normal_queue.front; cur != NULL; cur = cur->next) {
        if (!first) putchar(',');
        print_patient(&cur->data);
        first = 0;
    }

    printf("],\"emergencyQueue\":[");
    first = 1;
    for (PQNode *cur = emergency_queue.head; cur != NULL; cur = cur->next) {
        if (!first) putchar(',');
        print_patient(&cur->data);
        first = 0;
    }

    printf("],\"resources\":");
    print_resources();
    printf("}\n");
}

static void print_error(const char *message)
{
    printf("{\"ok\":false,\"error\":");
    json_string(message);
    printf("}\n");
}

static void cleanup(void)
{
    pq_free(&emergency_queue);
    q_free(&normal_queue);
    ll_free(&patients);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_error("Missing operation.");
        return 1;
    }

    load_state();

    if (strcmp(argv[1], "list") == 0) {
        print_list();
    } else if (strcmp(argv[1], "search") == 0) {
        int id;
        if (argc != 3 || !parse_positive_int(argv[2], &id)) {
            print_error("Patient ID must be a positive integer.");
            cleanup();
            return 1;
        }
        Patient *found = searchPatient(&hash_table, id);
        if (found == NULL) {
            print_error("Patient not found.");
            cleanup();
            return 2;
        }
        printf("{\"ok\":true,\"patient\":");
        print_patient(found);
        printf("}\n");
    } else if (strcmp(argv[1], "register") == 0) {
        int id, age, severity;
        if (argc != 7 ||
            !parse_positive_int(argv[2], &id) ||
            !parse_int_range(argv[4], 0, 120, &age) ||
            !parse_int_range(argv[5], 1, 5, &severity) ||
            argv[3][0] == '\0' || argv[6][0] == '\0' ||
            strlen(argv[3]) >= sizeof(((Patient *)0)->name) ||
            strlen(argv[6]) >= sizeof(((Patient *)0)->department) ||
            contains_forbidden_separator(argv[3]) ||
            contains_forbidden_separator(argv[6])) {
            print_error("Invalid patient details. Use a positive numeric ID, age 0-120, severity 1-5, and shorter text fields without |.");
            cleanup();
            return 1;
        }
        if (searchPatient(&hash_table, id) != NULL) {
            print_error("This Patient ID already exists.");
            cleanup();
            return 2;
        }

        Patient p;
        memset(&p, 0, sizeof(p));
        p.patient_id = id;
        p.age = age;
        p.severity = severity;
        strcpy(p.name, argv[3]);
        strcpy(p.department, argv[6]);

        if (ll_insert(&patients, p) != 0) {
            print_error("Could not add patient to the linked list.");
            cleanup();
            return 1;
        }
        insertPatient(&hash_table, p);
        if (severity <= 2) {
            if (pq_insert(&emergency_queue, p) != 0) {
                (void)ll_delete(&patients, id);
                print_error("Could not add patient to the emergency queue.");
                cleanup();
                return 1;
            }
        } else if (q_enqueue(&normal_queue, p) != 0) {
            (void)ll_delete(&patients, id);
            print_error("Could not add patient to the normal queue.");
            cleanup();
            return 1;
        }

        if (fh_save(PATIENTS_FILE, patients) < 0) {
            print_error("Could not save patient records.");
            cleanup();
            return 1;
        }
        printf("{\"ok\":true,\"message\":\"Patient registered successfully.\"}\n");
    } else if (strcmp(argv[1], "assign") == 0 || strcmp(argv[1], "release") == 0) {
        int id;
        if (argc != 3 || !parse_positive_int(argv[2], &id)) {
            print_error("Patient ID must be a positive integer.");
            cleanup();
            return 1;
        }
        if (searchPatient(&hash_table, id) == NULL) {
            print_error("Patient not found.");
            cleanup();
            return 2;
        }

        int index = -1;
        for (int i = 0; i < bed_count; ++i)
            if (bed_ids[i] == id) { index = i; break; }

        if (strcmp(argv[1], "assign") == 0) {
            if (index >= 0) {
                print_error("This patient already has a bed.");
                cleanup();
                return 2;
            }
            if (!allocateBed(&resources)) {
                print_error("No beds are available.");
                cleanup();
                return 2;
            }
            bed_ids[bed_count++] = id;
            if (!save_beds()) {
                --bed_count;
                (void)releaseBed(&resources);
                print_error("Could not save bed assignment.");
                cleanup();
                return 1;
            }
            printf("{\"ok\":true,\"message\":\"Bed assigned successfully.\"}\n");
        } else {
            if (index < 0) {
                print_error("This patient does not have an assigned bed.");
                cleanup();
                return 2;
            }
            for (int i = index; i < bed_count - 1; ++i)
                bed_ids[i] = bed_ids[i + 1];
            --bed_count;
            (void)releaseBed(&resources);
            if (!save_beds()) {
                print_error("Could not save bed release.");
                cleanup();
                return 1;
            }
            printf("{\"ok\":true,\"message\":\"Bed released successfully.\"}\n");
        }
    } else {
        print_error("Unknown operation.");
        cleanup();
        return 1;
    }

    cleanup();
    return 0;
}
