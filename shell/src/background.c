#include "background.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_JOBS 100

Job jobs[MAX_JOBS];
int job_count = 0;

int is_background_command(char *input) {
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '&') {
        input[len - 1] = '\0'; // Remove the '&'
        while (len > 1 && input[len - 2] == ' ') {
            input[len - 2] = '\0'; // Trim trailing spaces
            len--;
        }
        return 1;
    }
    return 0;
}

void execute_background(char *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return;
    } else if (pid == 0) {
        // Child process
        setsid(); // Detach from terminal input
        char *args[] = {"/bin/sh", "-c", cmd, NULL};
        execvp(args[0], args);
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if (job_count < MAX_JOBS) {
            jobs[job_count].job_id = job_count + 1;
            jobs[job_count].pid = pid;
            strncpy(jobs[job_count].command, cmd, sizeof(jobs[job_count].command) - 1);
            jobs[job_count].command[sizeof(jobs[job_count].command) - 1] = '\0';

            printf("[%d] %d\n", jobs[job_count].job_id, pid);
            job_count++;
        }
    }
}

void check_background_jobs() {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Find which job finished
        for (int i = 0; i < job_count; i++) {
            if (jobs[i].pid == pid) {
                if (WIFEXITED(status)) {
                    printf("%s with pid %d exited normally\n", jobs[i].command, pid);
                } else {
                    printf("%s with pid %d exited abnormally\n", jobs[i].command, pid);
                }

                // Shift jobs array down
                for (int j = i; j < job_count - 1; j++) {
                    jobs[j] = jobs[j + 1];
                }
                job_count--;
                break;
            }
        }
    }
}

void check_background_processes() {
    check_background_jobs(); // Alias for compatibility
}

void add_background_job(pid_t pid, const char *cmd) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].job_id = job_count + 1;
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, cmd, sizeof(jobs[job_count].command) - 1);
        jobs[job_count].command[sizeof(jobs[job_count].command) - 1] = '\0';
        printf("[%d] %d\n", jobs[job_count].job_id, pid);
        job_count++;
    }
}

void add_stopped_job(pid_t pid) {
    // Find if this job is already in our list
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            printf("\n[%d] Stopped %s\n", jobs[i].job_id, jobs[i].command);
            return;
        }
    }
}

void kill_all_background_jobs() {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid > 0) {
            kill(jobs[i].pid, SIGKILL);
        }
    }
}
