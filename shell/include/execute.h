// File: execute.h
#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdbool.h> // For the 'bool' type

/**
 * @brief Executes a single command group.
 *
 * This function is the heart of the shell's execution logic. It takes a
 * command string (which might contain pipes and redirections), parses it,
 * and executes it either as a built-in or an external process.
 *
 * @param command_str The command to execute (e.g., "ls -l > out.txt").
 * @param is_background True if the command should run in the background (&).
 */
int execute_command_group(char* command_str, bool is_background);

#endif