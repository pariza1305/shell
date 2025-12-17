#ifndef INPUT_H
#define INPUT_H

#include <stddef.h> // for size_t

// Reads user input into `input` buffer of given `size`
// Removes newline if present
void get_user_input(char *input, size_t size);

#endif // INPUT_H
