#include "builtins.h" 
//  code from llm starts
// REQUIRED: You must add these includes here
// to fix the "undeclared function" error.
#include "reveal.h"
#include "hop.h"
#include "log.h"
#include "exit.h" 
#include "parser.h"
#include <string.h> 
#include <stdio.h>
#include <stdlib.h> // For exit() in the exit.c helper

/**
 * is_builtin() function (This is the same as before)
 * It just checks the name.
 */
int is_builtin(const char *cmd) {
    if (cmd == NULL) {
        return 0;
    }
    if (strcmp(cmd, "reveal") == 0 ||
        strcmp(cmd, "hop") == 0    ||
        strcmp(cmd, "log") == 0    ||
        strcmp(cmd, "exit") == 0) 
    {
        return 1; 
    }
    return 0; 
}


/**
 * execute_builtin() (This is the UPDATED version)
 *
 * This version now calls your functions by their real names
 * and passes BOTH cmd->argc AND cmd->argv.
 */
void execute_builtin(command_t *cmd) {
    
    if (strcmp(cmd->argv[0], "reveal") == 0) {
        // Calls the "reveal" function from reveal.h
        reveal(cmd->argc, cmd->argv); 
    } 
    else if (strcmp(cmd->argv[0], "hop") == 0) {
        // Calls the "cmd_hop" function from hop.h
        cmd_hop(cmd->argc, cmd->argv);
    } 
    else if (strcmp(cmd->argv[0], "log") == 0) {
        // Calls the "handle_log_command" function from log.h
        handle_log_command(cmd->argc, cmd->argv);
    }
    else if (strcmp(cmd->argv[0], "exit") == 0) {
        // Calls the "run_exit" function from exit.h (we should standardize this one)
        run_exit(cmd->argv); // Our exit function only needs argv
    }
}
// code from llm ends