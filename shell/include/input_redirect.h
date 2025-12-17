#ifndef INPUT_REDIRECTION_H
#define INPUT_REDIRECTION_H

// This function handles input redirection.
// It will be called inside the child process.
// Returns 0 on success, -1 on failure.
int handle_input_redirection(const char *filename);

#endif // INPUT_REDIRECTION_H
