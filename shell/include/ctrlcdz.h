// llm code starts 
#ifndef CTRLCDZ_H
#define CTRLCDZ_H

#include <sys/types.h>

extern pid_t foreground_pid;   // track the current foreground job

void setup_signal_handlers();  // install all handlers
void sigint_handler(int sig);  // Ctrl-C
void sigtstp_handler(int sig); // Ctrl-Z
void handle_eof();             // Ctrl-D (EOF handling)

#endif
// llm code ends