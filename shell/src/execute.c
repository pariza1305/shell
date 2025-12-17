#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"
#include <fcntl.h>
#include <limits.h>
#include "execute.h"
#include "parser.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "cmd_piping.h"
#include "input_redirect.h"
#include "output_redirect.h"
#include "activities.h"
#include "fg_bg.h"
#include "ping.h"
#include "ctrlcdz.h"
#include "background.h"

int execute_command_group(char* command_str, bool is_background) {
    command_str = trim(command_str);
    command_t commands[32];
    int num_commands = 0;

    if (process_command(command_str, commands, &num_commands) <= 0) {
        return -1;
    }

    if (num_commands == 0) {
        return 0;
    }

     // 1. Check for built-in commands (which are never piped).
    if (num_commands == 1 && !is_background) {
        char** argv = commands[0].argv;
        int argc = 0;
        if (argv) { while(argv[argc] != NULL) argc++; }

        if (argc > 0) {
            // Handle redirection for built-in commands
            int saved_stdout = -1, saved_stdin = -1;
            
            if (commands[0].num_output_files > 0) {
                saved_stdout = dup(STDOUT_FILENO);
                // Check all output files for writability first
                for (int i = 0; i < commands[0].num_output_files; i++) {
                    int test_fd;
                    if (commands[0].output_types[i] == REDIRECT_TYPE_APPEND) {
                        test_fd = open(commands[0].output_files[i], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    } else {
                        test_fd = open(commands[0].output_files[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    }
                    if (test_fd < 0) {
                        printf("Unable to create file for writing\n");
                        if (saved_stdout >= 0) close(saved_stdout);
                        return -1;
                    }
                    close(test_fd);
                }
                // Use the last output redirection
                int last_output = commands[0].num_output_files - 1;
                if (handle_output_redirection(commands[0].output_files[last_output], commands[0].output_types[last_output]) < 0) {
                    if (saved_stdout >= 0) close(saved_stdout);
                    return -1;
                }
            }
            
            if (commands[0].num_input_files > 0) {
                saved_stdin = dup(STDIN_FILENO);
                // Check all input files for existence first
                for (int i = 0; i < commands[0].num_input_files; i++) {
                    int test_fd = open(commands[0].input_files[i], O_RDONLY);
                    if (test_fd < 0) {
                        perror(commands[0].input_files[i]);
                        if (saved_stdout >= 0) {
                            dup2(saved_stdout, STDOUT_FILENO);
                            close(saved_stdout);
                        }
                        if (saved_stdin >= 0) close(saved_stdin);
                        return -1;
                    }
                    close(test_fd);
                }
                // Use the last input redirection
                int last_input = commands[0].num_input_files - 1;
                if (handle_input_redirection(commands[0].input_files[last_input]) < 0) {
                    if (saved_stdout >= 0) {
                        dup2(saved_stdout, STDOUT_FILENO);
                        close(saved_stdout);
                    }
                    if (saved_stdin >= 0) close(saved_stdin);
                    return -1;
                }
            }
            
            int result = 0;
            if (strcmp(argv[0], "hop") == 0) {
                cmd_hop(argc, argv);
            }
            else if (strcmp(argv[0], "log") == 0) {
                handle_log_command(argc, argv);
            }
            else if (strcmp(argv[0], "reveal") == 0) {
                reveal(argc, argv);
            }
            else if (strcmp(argv[0], "activities") == 0) {
                activities();
            }
            else if (strcmp(argv[0], "ping") == 0) {
                ping_command(argc, argv);
            }
            else if (strcmp(argv[0], "fg") == 0) {
                fg_command(argc, argv);
            }
            else if (strcmp(argv[0], "bg") == 0) {
                bg_command(argc, argv);
            }
            else {
                result = 1; // Not a built-in, continue to external command execution
            }
            
            // Restore redirections
            if (saved_stdout >= 0) {
                dup2(saved_stdout, STDOUT_FILENO);
                close(saved_stdout);
            }
            if (saved_stdin >= 0) {
                dup2(saved_stdin, STDIN_FILENO);
                close(saved_stdin);
            }
            
            if (result == 0) return 0; // Built-in was executed
        }
    }
    // 2. Handle external commands.
    if (num_commands == 1) {
        // Validate all redirections before forking
        if (commands[0].num_input_files > 0) {
            for (int i = 0; i < commands[0].num_input_files; i++) {
                int test_fd = open(commands[0].input_files[i], O_RDONLY);
                if (test_fd < 0) {
                    perror(commands[0].input_files[i]);
                    return -1;
                }
                close(test_fd);
            }
        }
        
        if (commands[0].num_output_files > 0) {
            for (int i = 0; i < commands[0].num_output_files; i++) {
                int test_fd;
                if (commands[0].output_types[i] == REDIRECT_TYPE_APPEND) {
                    test_fd = open(commands[0].output_files[i], O_WRONLY | O_CREAT | O_APPEND, 0644);
                } else {
                    test_fd = open(commands[0].output_files[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                if (test_fd < 0) {
                    printf("Unable to create file for writing\n");
                    return -1;
                }
                close(test_fd);
            }
        }
        
        // --- SINGLE EXTERNAL COMMAND ---
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return -1; // Fork failed
        }

        if (pid == 0) { // --- CHILD PROCESS ---
            // Set process group for proper signal handling
            setpgid(0, 0);
            
            // 1. Handle I/O Redirection - use last redirection for each type
            if (commands[0].num_input_files > 0) {
                int last_input = commands[0].num_input_files - 1;
                if (handle_input_redirection(commands[0].input_files[last_input]) < 0) {
                    exit(1); // Exit if redirection fails
                }
            }
            if (commands[0].num_output_files > 0) {
                int last_output = commands[0].num_output_files - 1;
                if (handle_output_redirection(commands[0].output_files[last_output], commands[0].output_types[last_output]) < 0) {
                    exit(1); // Exit if redirection fails
                }
            }
            
            // 2. Execute the command
            execvp(commands[0].argv[0], commands[0].argv);

            // If execvp returns, it means an error occurred.
            printf("Command not found!\n");
            exit(127);
        } else { // --- PARENT PROCESS ---
            if (!is_background) {
                foreground_pid = pid; // Set foreground PID for signal handling
                int status;
                waitpid(pid, &status, WUNTRACED);
                foreground_pid = -1; // Reset foreground PID
                
                // Clear any EOF condition on stdin that might have been set
                // during the child process execution (e.g., wc -c receiving Ctrl+D)
                if (feof(stdin)) {
                    clearerr(stdin);
                }
            } else {
                // Add to background jobs
                add_background_job(pid, command_str);
            }
            // code from llm ends 
        }
    } else {
        // --- PIPELINE OF COMMANDS ---
        run_pipeline(commands, num_commands);
    }

    return 0; // Success
}

