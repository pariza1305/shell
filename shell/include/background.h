#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <sys/types.h>

// Struct for storing background job info
typedef struct {
    int job_id;
    pid_t pid;
    char command[256];
} Job;

// Extern globals
extern Job jobs[100];
extern int job_count;

// Functions
void check_background_processes();         // Reap finished jobs  
void check_background_jobs();              // Alias for compatibility
void execute_background(char *cmd);        // Run command in background
int is_background_command(char *input);    // Detect & and strip it
void add_background_job(pid_t pid, const char *cmd); // Add background job
void add_stopped_job(pid_t pid);           // Add stopped job
void kill_all_background_jobs();           // Kill all background jobs

#endif
