#ifndef PARSER_H
#define PARSER_H

#include "output_redirect.h"   // reuse RedirectionType

#define MAX_REDIRECTS 16

typedef struct {
    int argc ;
    char **argv;
    
    // Arrays to hold ALL redirect files
    char *input_files[MAX_REDIRECTS];
    int num_input_files;
    
    char *output_files[MAX_REDIRECTS];
    RedirectionType output_types[MAX_REDIRECTS]; // Store APPEND/TRUNCATE for each file
    int num_output_files;

} command_t;

// --- Function declarations ---
int tokenize(char *input, char **tokens, int max_tokens);
int process_command(char *input, command_t *commands, int *num_commands_out);

#endif // PARSER_H
