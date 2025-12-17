// File: hop.c
#include "hop.h"
#include "prompt.h"
#include "parser.h" // Assuming this has tokenize prototype
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// These are now private to hop.c and shared via the getter functions.
static char previous_directory[PATH_MAX] = {0};
static bool has_been_set = false;

// Getter function for other modules to use.
const char* get_previous_directory(void) {
    return previous_directory;
}

// Getter function to check if the variable is valid.
int has_previous_directory_been_set(void) {
    return has_been_set;
}

// The rest of your hop logic from before...
void cmd_hop(int argc, char **argv) {
    char current_directory[PATH_MAX];
    if (argc == 1) { // hop with no args
        if (getcwd(current_directory, sizeof(current_directory)) != NULL) {
            if (chdir(get_shell_virtual_home()) == 0) {
                strncpy(previous_directory, current_directory, PATH_MAX);
                has_been_set = true;
            } else { printf("No such directory!\n"); }
        }
        return;
    }
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (getcwd(current_directory, sizeof(current_directory)) == NULL) continue;
        int chdir_result = -1;
        if (strcmp(arg, "~") == 0) chdir_result = chdir(get_shell_virtual_home());
        else if (strcmp(arg, ".") == 0) continue;
        else if (strcmp(arg, "..") == 0) chdir_result = chdir("..");
        else if (strcmp(arg, "-") == 0) {
            if (!has_been_set) continue;
            chdir_result = chdir(previous_directory);
        } else chdir_result = chdir(arg);

        if (chdir_result == 0) {
            strncpy(previous_directory, current_directory, PATH_MAX);
            has_been_set = true;
        } else { printf("No such directory!\n"); }
    }
}

// We need a tokenizer prototype if parser.h is not fully defined yet.
int tokenize(char *input, char **tokens, int max_tokens);

int handle_hop(char *input) {
    char *input_copy = strdup(input);
    if (!input_copy) return 0;
    char *tokens[128];
    int count = tokenize(input_copy, tokens, 128);
    if (count > 0 && strcmp(tokens[0], "hop") == 0) {
        cmd_hop(count, tokens);
        for (int i = 0; i < count; i++) free(tokens[i]);
        free(input_copy);
        return 1;
    }
    for (int i = 0; i < count; i++) free(tokens[i]);
    free(input_copy);
    return 0;
}