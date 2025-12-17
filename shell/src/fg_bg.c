#include "fg_bg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

typedef enum { RUNNING, STOPPED } job_state;

typedef struct {
    int job_number;
    pid_t pid;
    char command[256];
    job_state state;
} job_t;

static job_t jobs[100];   // job table
static int job_count = 0;
static int last_job = -1; // index of most recent bg/stopped job

// ---------- Helper functions ----------
static void add_job(pid_t pid, const char *cmd, job_state state) {
    jobs[job_count].job_number = job_count + 1;
    jobs[job_count].pid = pid;
    jobs[job_count].state = state;
    strncpy(jobs[job_count].command, cmd, sizeof(jobs[job_count].command));
    last_job = job_count;
    job_count++;
}

static int find_job(int job_number) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].job_number == job_number)
            return i;
    }
    return -1;
}

// ---------- fg command ----------
void fg_cmd(int argc, char **argv) {
    int job_index;

    if (argc < 2) {
        if (last_job == -1) {
            printf("No such job\n");
            return;
        }
        job_index = last_job;
    } else {
        int job_number = atoi(argv[1]);
        job_index = find_job(job_number);
        if (job_index == -1) {
            printf("No such job\n");
            return;
        }
    }

    pid_t pid = jobs[job_index].pid;
    printf("%s\n", jobs[job_index].command);

    // Resume if stopped
    if (jobs[job_index].state == STOPPED) {
        kill(-pid, SIGCONT);
    }

    jobs[job_index].state = RUNNING;

    // Wait until process exits or stops
    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
        perror("waitpid failed");
        return;
    }

    if (WIFSTOPPED(status)) {
        jobs[job_index].state = STOPPED;
    } else {
        // Process finished → remove it
        jobs[job_index].state = RUNNING; // optional: clear entry
    }
}

// ---------- bg command ----------
void bg_cmd(int argc, char **argv) {
    int job_index;

    if (argc < 2) {
        if (last_job == -1) {
            printf("No such job\n");
            return;
        }
        job_index = last_job;
    } else {
        int job_number = atoi(argv[1]);
        job_index = find_job(job_number);
        if (job_index == -1) {
            printf("No such job\n");
            return;
        }
    }

    if (jobs[job_index].state == RUNNING) {
        printf("Job already running\n");
        return;
    }

    pid_t pid = jobs[job_index].pid;
    if (kill(-pid, SIGCONT) == -1) {
        perror("kill failed");
        return;
    }

    jobs[job_index].state = RUNNING;
    printf("[%d] %s &\n", jobs[job_index].job_number, jobs[job_index].command);
}

// ---------- jobs command ----------
void jobs_cmd(int argc, char **argv) {
    (void)argc; (void)argv; // unused
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %s (%s)\n", jobs[i].job_number, jobs[i].command,
               jobs[i].state == RUNNING ? "Running" : "Stopped");
    }
}

// ---------- Example process creation (background) ----------
void process_creation() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        setpgid(0, 0); // new process group
        char *args[] = {"/bin/sleep", "30", NULL}; // dummy cmd
        if (execv(args[0], args) == -1) {
            perror("execv failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent → don't wait (background job)
        add_job(pid, "sleep 30", RUNNING);
        printf("[%d] %d\n", jobs[job_count - 1].job_number, pid);
    }
}

// Wrapper functions for execute.c
void fg_command(int argc, char **argv) {
    fg_cmd(argc, argv);
}

void bg_command(int argc, char **argv) {
    bg_cmd(argc, argv);
}
