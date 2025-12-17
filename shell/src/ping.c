#include "ping.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

void ping_cmd(int pid , int signal_no)
{
    int actual_signal = signal_no % 32;  // requirement

    if (kill(pid, actual_signal) == -1) {
        if (errno == ESRCH) {
            printf("No such process found\n");
        } else {
            perror("kill failed");
        }
    } else {
        printf("Sent signal %d to process with pid %d\n", signal_no, pid);
    }
}

// Wrapper function for execute.c
void ping_command(int argc, char **argv) {
    if (argc != 3) {
        printf("Invalid syntax!\n");
        return;
    }
    
    char *endptr;
    int pid = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0') {
        printf("Invalid syntax!\n");
        return;
    }
    
    int signal_no = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0') {
        printf("Invalid syntax!\n");
        return;
    }
    
    ping_cmd(pid, signal_no);
}