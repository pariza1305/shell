#include "output_redirect.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int handle_output_redirection(const char *filename, RedirectionType type) {
    if (!filename || type == REDIRECT_TYPE_NONE) return 0;

    int flags = O_WRONLY | O_CREAT;
    if (type == REDIRECT_TYPE_APPEND)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;

    int fd = open(filename, flags, 0666);
    if (fd < 0) {
        printf("Unable to create file for writing\n");
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) < 0) {
        perror("dup2");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
