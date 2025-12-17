#include "activities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static int cmp_jobs(const void *a, const void *b) {
    return strcmp(((Job*)a)->command, ((Job*)b)->command);
}

void activities() {
    if (job_count == 0) {
        printf("No active background jobs.\n");
        return;
    }

    // Copy jobs[] for sorting
    Job sorted[job_count];
    memcpy(sorted, jobs, sizeof(Job) * job_count);
    qsort(sorted, job_count, sizeof(Job), cmp_jobs);

    for (int i = 0; i < job_count; i++) {
        int status;
        pid_t result = waitpid(sorted[i].pid, &status,
                               WNOHANG | WUNTRACED | WCONTINUED);

        if (result == 0) {
            // Process still running
            printf("[%d] : %s - Running\n",
                   sorted[i].pid, sorted[i].command);
        } else if (result > 0 && WIFSTOPPED(status)) {
            printf("[%d] : %s - Stopped\n",
                   sorted[i].pid, sorted[i].command);
        }
        // If process exited, check_background_jobs()
        // should have already removed it from jobs[]
    }
}
