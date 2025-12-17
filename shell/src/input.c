#include "input.h"
#include <stdio.h>
#include <string.h>

void get_user_input(char *input, size_t size) {
    if (fgets(input, size, stdin) != NULL) {
        // Remove trailing newline
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
    }
}