#ifndef LOG_H
#define LOG_H

// Initializes the log system by loading history from the file.
// This should be called once when the shell starts.
void log_init(void);

// Saves the current command history to the file.
// This should be called once when the shell exits.
void log_cleanup(void);

// Adds a command to the history log, handling all rules (no duplicates, no "log").
void add_to_log(const char *command);

// The dispatcher function that checks for and handles the "log" command.
void handle_log_command(int argc, char **argv);

void print_log();
void purge_log();
void execute_from_log(int index);

// A placeholder for the main execution function you will build in Part C.
// We declare it here so `log.c` knows it exists.
void execute_full_command_line(const char *command);

#endif // LOG_H
