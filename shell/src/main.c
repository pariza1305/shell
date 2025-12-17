#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "execute.h"
#include "prompt.h"
#include "ctrlcdz.h"
#include "background.h"
#include "log.h"

int main() {
    init_shell();
    log_init(); // Initialize the command history system
    setup_signal_handlers(); // Set up signal handlers for Ctrl+C, Ctrl+Z

    char input[1024];

    while (1) {
        check_background_processes(); // Check for completed background jobs
        show_prompt();
        if (fgets(input, sizeof(input), stdin) == NULL) {
            handle_eof(); // Handle Ctrl-D properly
            break;
        }
        // codefrom llm starts 
        // --- NEW ROBUST PARSING LOGIC ---
        // This loop manually processes the input string to correctly handle both '&' and ';'
        char* current_command = input;
        while (current_command != NULL && *current_command != '\0') {
            // Find the next separator ('&' or ';')
            char* separator = strpbrk(current_command, "&;");
            bool is_background = false;

            if (separator != NULL) {
                if (*separator == '&') {
                    is_background = true; // This command should run in the background
                }
                // Terminate the current command string at the separator
                *separator = '\0';
            }

            // Execute the single command group we just isolated
            if (strlen(current_command) > 0) {
                // Trim whitespace and add to log if it's not empty
                char *trimmed = current_command;
                while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
                if (*trimmed != '\0') {
                    // Remove trailing whitespace
                    char *end = trimmed + strlen(trimmed) - 1;
                    while (end > trimmed && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
                    end[1] = '\0';
                    
                    add_to_log(trimmed); // Add command to history before execution
                }
            }
            
            if (execute_command_group(current_command, is_background) != 0) {
                // An invalid syntax error occurred, stop processing the rest of the line.
                break;
            }

            // Move the pointer to the start of the next command
            if (separator != NULL) {
                current_command = separator + 1;
            } else {
                current_command = NULL; // No more commands on this line
            }
        }
        // cpde from llm ends
    }
    
    log_cleanup(); // Save history and cleanup
    return 0;
}