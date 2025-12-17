#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h> // Added for open(), O_RDONLY, O_WRONLY, etc.
#include "output_redirect.h"
#include "input_redirect.h"
#include "parser.h"
#include "builtins.h"
static void create_pipes(int *pipefd , int n) {
    for (int i = 0 ; i < n-1 ; i++) {
        if (pipe(pipefd + i*2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
}

static void close_pipes(int *pipefd, int n) {
    for (int i = 0 ; i < n-1 ; i++) {
        close(pipefd[i*2]);
        close(pipefd[i*2 + 1]);
    }
}

// In src/cmd_piping.c

static void execute_cmd(int i, int n, int *pipefd, command_t *cmds) {
    // 1. Set up stdin from the previous command's pipe
    if (i > 0) {
        if (dup2(pipefd[(i - 1) * 2], STDIN_FILENO) < 0) {
            perror("dup2 stdin");
            exit(1);
        }
    }
    // 2. Set up stdout to the next command's pipe
    if (i < n - 1) {
        if (dup2(pipefd[i * 2 + 1], STDOUT_FILENO) < 0) {
            perror("dup2 stdout");
            exit(1);
        }
    }
    // close all original pipe fds
    close_pipes(pipefd, n);

    // Handle multiple input redirects: validate all, then use last
    if (cmds[i].num_input_files > 0) {
        for (int k = 0; k < cmds[i].num_input_files; k++) {
            int fd = open(cmds[i].input_files[k], O_RDONLY);
            if (fd < 0) { perror(cmds[i].input_files[k]); exit(1);} else close(fd);
        }
        int last = cmds[i].num_input_files - 1;
        if (handle_input_redirection(cmds[i].input_files[last]) < 0) exit(1);
    }
    // Handle multiple output redirects: validate all, then use last
    if (cmds[i].num_output_files > 0) {
        for (int k = 0; k < cmds[i].num_output_files; k++) {
            int fd;
            if (cmds[i].output_types[k] == REDIRECT_TYPE_APPEND)
                fd = open(cmds[i].output_files[k], O_WRONLY|O_CREAT|O_APPEND, 0644);
            else
                fd = open(cmds[i].output_files[k], O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd < 0) { printf("Unable to create file for writing\n"); exit(1);} else close(fd);
        }
        int last = cmds[i].num_output_files - 1;
        if (handle_output_redirection(cmds[i].output_files[last], cmds[i].output_types[last]) < 0) exit(1);
    }

    if (!cmds[i].argv || !cmds[i].argv[0]) {
        exit(0);
    }

    if (is_builtin(cmds[i].argv[0])) {
        execute_builtin(&cmds[i]);
        exit(EXIT_SUCCESS);
    } else {
        execvp(cmds[i].argv[0], cmds[i].argv);
        fprintf(stderr, "Command not found!\n");
        exit(127);
    }
}


void run_pipeline(command_t *commands, int n) {
    int pipefd[2*(n-1)];
    create_pipes(pipefd, n);
    int pids[n];

    for (int i = 0; i < n; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            execute_cmd(i, n, pipefd, commands);
        }
    }

    close_pipes(pipefd, n);

    for (int i = 0; i < n; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    // Clear any EOF condition on stdin after pipeline completion
    if (feof(stdin)) {
        clearerr(stdin);
    }
}
