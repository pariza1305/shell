#include "ctrlcdz.h"
#include "background.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

pid_t foreground_pid = -1;

void setup_signal_handlers() {
    struct sigaction sa_int, sa_tstp;
    
    // Set up SIGINT handler (Ctrl+C)
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);
    
    // Set up SIGTSTP handler (Ctrl+Z)
    sa_tstp.sa_handler = sigtstp_handler;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa_tstp, NULL);
}

void sigint_handler(int sig) {
    if (foreground_pid > 0) {
        kill(-foreground_pid, SIGINT);  // send SIGINT to foreground process group
        write(STDOUT_FILENO, "\n", 1);  // Print newline
    }
}

void sigtstp_handler(int sig) {
    if (foreground_pid > 0) {
        kill(-foreground_pid, SIGTSTP);  // stop foreground job
        // Print suspend message immediately
        write(STDOUT_FILENO, "\n[1] Stopped\n", 12);
        // Move to background jobs list
        add_stopped_job(foreground_pid);
        foreground_pid = -1;
    }
}

void handle_eof() {
    printf("logout\n");
    // Kill all background jobs before exiting
    kill_all_background_jobs();
    exit(0);
}