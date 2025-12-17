#include <ctype.h>
#include <string.h>
#include "utils.h"

const char *skip_spaces(const char *s) {
    while (*s == ' ' || *s == '\t') s++;
    return s;
}

char *trim(char *s) {
    if (s == NULL) return s;

    // left trim
    while (*s && isspace((unsigned char)*s)) s++;

    // right trim
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return s;
}
