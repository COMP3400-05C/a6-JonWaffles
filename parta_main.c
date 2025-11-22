#include "parta.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static int usage_error(void) {
    printf("ERROR: Missing arguments\n");
    return 1;
}

static int parse_int(const char* text, int* out) {
    if (text == NULL || out == NULL || *text == '\0') {
        return 0;
    }

    char* endptr = NULL;
    long value = strtol(text, &endptr, 10);
    if (*endptr != '\0') {
        return 0;
    }

    *out = (int)value;
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return usage_error();
    }

    const char* algo = argv[1];
    if (strcmp(algo, "fcfs") == 0) {
        if (argc < 3) {
            return usage_error();
        }

        int plen = argc - 2;
        int* bursts = malloc(sizeof(int) * plen);
        if (bursts == NULL) {
            fprintf(stderr, "ERROR: Unable to allocate bursts\n");
            return 1;
        }
        for (int i = 0; i < plen; ++i) {
            if (!parse_int(argv[i + 2], &bursts[i])) {
                free(bursts);
                return usage_error();
            }
        }

        struct pcb* procs = init_procs(bursts, plen);
        free(bursts);
        if (procs == NULL) {
            fprintf(stderr, "ERROR: Unable to allocate processes\n");
            return 1;
        }

        printf("Using FCFS\n\n");
        for (int i = 0; i < plen; ++i) {
            printf("Accepted P%d: Burst %d\n", i, procs[i].burst_left);
        }
        int total_time = fcfs_run(procs, plen);
        (void)total_time; // total time isn't printed, but keep for completeness

        long wait_sum = 0;
        for (int i = 0; i < plen; ++i) {
            wait_sum += procs[i].wait;
        }
        double avg_wait = (plen > 0) ? (double)wait_sum / (double)plen : 0.0;
        printf("Average wait time: %.2f\n", avg_wait);

        free(procs);
        return 0;
    }

    if (strcmp(algo, "rr") == 0) {
        if (argc < 4) {
            return usage_error();
        }

        int quantum = 0;
        if (!parse_int(argv[2], &quantum) || quantum <= 0) {
            return usage_error();
        }

        int plen = argc - 3;
        int* bursts = malloc(sizeof(int) * plen);
        if (bursts == NULL) {
            fprintf(stderr, "ERROR: Unable to allocate bursts\n");
            return 1;
        }
        for (int i = 0; i < plen; ++i) {
            if (!parse_int(argv[i + 3], &bursts[i])) {
                free(bursts);
                return usage_error();
            }
        }

        struct pcb* procs = init_procs(bursts, plen);
        free(bursts);
        if (procs == NULL) {
            fprintf(stderr, "ERROR: Unable to allocate processes\n");
            return 1;
        }

        printf("Using RR(%d)\n\n", quantum);
        for (int i = 0; i < plen; ++i) {
            printf("Accepted P%d: Burst %d\n", i, procs[i].burst_left);
        }
        int total_time = rr_run(procs, plen, quantum);
        (void)total_time;

        long wait_sum = 0;
        for (int i = 0; i < plen; ++i) {
            wait_sum += procs[i].wait;
        }
        double avg_wait = (plen > 0) ? (double)wait_sum / (double)plen : 0.0;
        printf("Average wait time: %.2f\n", avg_wait);

        free(procs);
        return 0;
    }

    return usage_error();
}
