#include "execute.h"
#include "log.h"
#include "parser.h" // For tokenize
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOG_MAX 15
#define LOG_FILE ".shell_log"

// Forward declaration for a function we will need from the main loop later.
void execute_full_command_line(const char *command);

// --- Module-Private (static) Globals ---
static char *log_buffer[LOG_MAX];
static int log_count = 0;
static int log_start = 0; // The index of the oldest command

// --- Module-Private (static) Helper Functions ---

// Loads the command history from the log file into memory.
static void load_log_from_file() {
  FILE *f = fopen(LOG_FILE, "r");
  if (!f) {
    return; // No log file exists yet, which is fine.
  }

  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }
    // Use the public add function to ensure logic is consistent
    add_to_log(line);
  }
  fclose(f);
}

// Saves the current in-memory command history to the log file.
static void save_log_to_file() {
  FILE *f = fopen(LOG_FILE, "w");
  if (!f) {
 
    return;
  }

  int index = log_start;
  for (int i = 0; i < log_count; i++) {
    fprintf(f, "%s\n", log_buffer[index]);
    index = (index + 1) % LOG_MAX;
  }
  fclose(f);
}

// Prints all commands currently in the log, from oldest to newest.
void print_log() {
  int current_index = log_start;
  for (int i = 0; i < log_count; i++) {
    printf("%s\n", log_buffer[current_index]);
    current_index = (current_index + 1) % LOG_MAX;
  }
}

// Clears all commands from the log.
void purge_log() {
  for (int i = 0; i < log_count; i++) {
    int index_to_free = (log_start + i) % LOG_MAX;
    free(log_buffer[index_to_free]);
    log_buffer[index_to_free] = NULL;
  }
  log_count = 0;
  log_start = 0;
  save_log_to_file(); // Save the now-empty log
}

// Executes a command from the log based on its 1-indexed (newest to oldest) position.
// ... now, find the execute_from_log function and REPLACE it with this:
void execute_from_log(int index) {
  if (index < 1 || index > log_count) {
    printf("Invalid log index: %d\n", index);
    return;
  }

  // Convert the 1-indexed "newest to oldest" into the circular buffer's
  // 0-indexed "oldest to newest" index.
  int actual_index = (log_start + log_count - index) % LOG_MAX;

  // We need a mutable copy of the command string
  char cmd_copy[1024];
  strncpy(cmd_copy, log_buffer[actual_index], sizeof(cmd_copy) - 1);
  cmd_copy[sizeof(cmd_copy) - 1] = '\0';

  printf("Executing: %s\n", cmd_copy);
  
  // Call the NEW function, not the old one.
  // A command from the log should run in the foreground, so is_background is false.
  execute_command_group(cmd_copy, false);
}


void log_init() { load_log_from_file(); }

void log_cleanup() {
  save_log_to_file();
  for (int i = 0; i < log_count; i++) {
    free(log_buffer[(log_start + i) % LOG_MAX]);
  }
}

void add_to_log(const char *cmd) {
  // Don't store log commands
  if (strncmp(cmd, "log", 3) == 0 && (cmd[3] == '\0' || cmd[3] == ' ')) {
    return;
  }

  // Don't store a command if it's identical to the last one
  if (log_count > 0) {
    int last_index = (log_start + log_count - 1) % LOG_MAX;
    if (strcmp(log_buffer[last_index], cmd) == 0) {
      return;
    }
  }

  // If the buffer is full, overwrite the oldest command
  if (log_count == LOG_MAX) {
    free(log_buffer[log_start]);
    log_buffer[log_start] = strdup(cmd);
    log_start = (log_start + 1) % LOG_MAX; // Move the start pointer
  } else {
    // Otherwise, add to the end
    int new_index = (log_start + log_count) % LOG_MAX;
    log_buffer[new_index] = strdup(cmd);
    log_count++;
  }
}


void handle_log_command(int argc, char **argv) {
    if (argc == 1) {
        print_log();
    } else if (argc == 2 && strcmp(argv[1], "purge") == 0) {
        purge_log();
    } else if (argc == 3 && strcmp(argv[1], "execute") == 0) {
        int index = atoi(argv[2]);
        execute_from_log(index);
    } else {
        // Invalid syntax for the log command
        printf("log: Invalid Syntax!\n");
    }
}