#ifndef HOP_H
#define HOP_H

void cmd_hop(int argc, char **argv);

// This function checks the input string for the "hop" command and handles it.
int handle_hop(char *input);

// Gets the last directory the user was in before the most recent 'hop'.
const char* get_previous_directory(void);

// Checks if a 'previous' directory has been set yet.
int has_previous_directory_been_set(void);

#endif // HOP_H