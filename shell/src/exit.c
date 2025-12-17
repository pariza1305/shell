#include "exit.h"
#include <stdlib.h> // For the real exit() function and atoi()
#include <stdio.h>  // For fprintf() and stderr
#include <string.h> // For strcmp()

/**
 * Executes the logic for the shell's built-in 'exit' command.
 */
void run_exit(char **argv) {
    int exit_status = 0; // Default exit status is 0 (success)

    if (argv[1] != NULL) {
        // User provided an argument (e.g., "exit 50")
        
        if (argv[2] != NULL) {
            // Too many arguments (e.g., "exit 50 100")
            fprintf(stderr, "exit: too many arguments\n");
            return; // This is an error, just return without exiting the shell
        }

        // Convert the string argument to an integer
        exit_status = atoi(argv[1]);
    }
    
    // Call the actual system exit() function to terminate the shell
    exit(exit_status);
}