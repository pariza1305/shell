#ifndef OUTPUT_REDIRECT_H
#define OUTPUT_REDIRECT_H

typedef enum {
    REDIRECT_TYPE_NONE,     // No output redirection
    REDIRECT_TYPE_TRUNCATE, // > (overwrite file)
    REDIRECT_TYPE_APPEND    // >> (append to file)
} RedirectionType;

int handle_output_redirection(const char *filename, RedirectionType type);

#endif
