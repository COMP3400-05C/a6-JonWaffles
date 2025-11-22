#include "parta.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * Create an array of PCBs from bursts. Each PCB is allocated on the heap and
 * initialized so burst_left equals the requested burst, pid equals index, and
 * wait starts at 0.
 */
struct pcb* init_procs(int* bursts, int blen) {
    if (bursts == NULL || blen <= 0) {
        return NULL;
    }

    struct pcb* procs = malloc(sizeof(struct pcb) * blen);
    if (procs == NULL) {
        return NULL;
    }

    for (int i = 0; i < blen; ++i) {
        procs[i].pid = i;
        procs[i].burst_left = bursts[i];
        procs[i].wait = 0;
    }

    return procs;
}

/**
 * Print array
 */
void printall(struct pcb* procs, int plen) {
    if (procs == NULL || plen <= 0) {
        return;
    }

    for (int i = 0; i < plen; ++i) {
        printf("PID %d: burst_left=%d wait=%d\n",
            procs[i].pid, procs[i].burst_left, procs[i].wait);
    }
}

/**
 * Run the specified process for up to amount units.
 */
void run_proc(struct pcb* procs, int plen, int current, int amount) {
    if (procs == NULL || plen <= 0 || current < 0 || current >= plen || amount <= 0) {
        return;
    }

    if (procs[current].burst_left <= 0) {
        return;
    }

    int run_time = amount;
    if (run_time > procs[current].burst_left) {
        run_time = procs[current].burst_left;
    }

    procs[current].burst_left -= run_time;

    for (int i = 0; i < plen; ++i) {
        if (i == current) {
            continue;
        }
        if (procs[i].burst_left > 0) {
            procs[i].wait += run_time;
        }
    }
}

/**
 * Run all processes sequentially (FCFS).
 */
int fcfs_run(struct pcb* procs, int plen) {
    if (procs == NULL || plen <= 0) {
        return 0;
    }

    int current_time = 0;
    for (int i = 0; i < plen; ++i) {
        if (procs[i].burst_left <= 0) {
            continue;
        }

        int run_time = procs[i].burst_left;
        run_proc(procs, plen, i, run_time);
        current_time += run_time;
    }

    return current_time;
}

/**
 * Return the next process index that still has burst remaining.
 */
int rr_next(int current, struct pcb* procs, int plen) {
    if (procs == NULL || plen <= 0) {
        return -1;
    }

    bool work_remaining = false;
    for (int i = 0; i < plen; ++i) {
        if (procs[i].burst_left > 0) {
            work_remaining = true;
            break;
        }
    }
    if (!work_remaining) {
        return -1;
    }

    int start = 0;
    if (current >= 0) {
        start = (current + 1) % plen;
    }

    for (int offset = 0; offset < plen; ++offset) {
        int idx = (start + offset) % plen;
        if (procs[idx].burst_left > 0) {
            return idx;
        }
    }

    return -1;
}

/**
 * Run processes
 */
int rr_run(struct pcb* procs, int plen, int quantum) {
    if (procs == NULL || plen <= 0 || quantum <= 0) {
        return 0;
    }

    int current_time = 0;
    int current = -1;
    while (1) {
        int next = rr_next(current, procs, plen);
        if (next == -1) {
            break;
        }

        int run_time = procs[next].burst_left < quantum ? procs[next].burst_left : quantum;
        run_proc(procs, plen, next, run_time);
        current_time += run_time;
        current = next;
    }

    return current_time;
}
