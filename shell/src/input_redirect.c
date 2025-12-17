#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include "input_redirect.h"

int handle_input_redirection(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
       perror(filename);
        return -1;
    }

    if (dup2(fd, STDIN_FILENO) < 0) {
        perror("dup2");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
