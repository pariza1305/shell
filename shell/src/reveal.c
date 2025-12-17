#include "reveal.h"
#include "prompt.h" // For virtual home
#include "hop.h"    // For previous directory
#include "parser.h" // For tokenizing
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// Comparison function for qsort, ensuring lexicographical (ASCII) order.
static int lexicographical_compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// The core logic for the 'reveal' command.
void reveal(int argc, char **argv) {
    bool show_all = false;  // Corresponds to -a flag
    bool line_mode = false; // Corresponds to -l flag
    char target_path[PATH_MAX];
    int path_arg_index = -1;

    // 1. Parse arguments: flags and the single path argument.
    for (int i = 1; i < argc; i++) {
        // Treat "-" by itself as a path, not a flag.
        if (strcmp(argv[i], "-") == 0) {
            path_arg_index = i;
            break;
        } 
        else if (argv[i][0] == '-') {
            // It's a flag, parse all its characters
            for (int j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'a') show_all = true;
                else if (argv[i][j] == 'l') line_mode = true;
                // Ignore other characters as per ls behavior
            }
        } else {
            // It's a path argument
            path_arg_index = i;
            break;
        }
    }

    // 2. Syntax Check: Ensure there's at most one path argument.
    if (path_arg_index != -1 && path_arg_index < argc - 1) {
        printf("reveal: Invalid Syntax!\n");
       // fflush(stdout); // <<< ADD THIS FLUSH
        return;
    }

    // 3. Determine the target path to reveal.
    if (path_arg_index == -1) {
        // No path argument, default to current directory "."
        strcpy(target_path, ".");
    } else {
        const char* path_arg = argv[path_arg_index];
        if (strcmp(path_arg, "~") == 0) {
            strcpy(target_path, get_shell_virtual_home());
        } else if (strcmp(path_arg, "-") == 0) {
            if (!has_previous_directory_been_set()) {
                printf("No such directory!\n");
                fflush(stdout); // <<< ADD THIS FLUSH
                return;
            }
            strcpy(target_path, get_previous_directory());
        } else {
            // Handles '.', '..', and any other name
            strcpy(target_path, path_arg);
        }
    }

    // 4. Read the directory entries.
    DIR *dir = opendir(target_path);
    if (dir == NULL) {
        printf("No such directory!\n");
        fflush(stdout); // <<< ADD THIS FLUSH    // debuuged by llm 
        return;
    }

    struct dirent *entry;
    char **names = NULL;
    size_t count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue; // Skip hidden files if -a is not set
        }
        names = realloc(names, sizeof(char*) * (count + 1));
        names[count++] = strdup(entry->d_name);
    }
    closedir(dir);
    
    if (count > 0) {
        qsort(names, count, sizeof(char*), lexicographical_compare);
    }

    // 5. Sort and print the entries.
    for (size_t k = 0; k < count; k++) {
        printf("%s", names[k]);
        if (line_mode) {
            printf("\n");
        } else {
            // Add a space unless it's the last item
            if (k < count - 1) printf(" ");
        }
    }
    // Add a final newline if not in line_mode and if there were entries
    if (!line_mode && count > 0) printf("\n");

    // 6. Clean up memory.
    for (size_t k = 0; k < count; k++) free(names[k]);
    free(names);
    
    fflush(stdout); // <<< ADD A FINAL FLUSH to ensure all output is sent
}