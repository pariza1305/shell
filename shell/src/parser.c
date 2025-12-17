#include "parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 128



// Checks if a token is any kind of operator
static bool is_operator(const char *tok) {
    if (tok == NULL) return false;
    return (strcmp(tok, "|") == 0 || strcmp(tok, "&") == 0 ||
            strcmp(tok, ";") == 0 || strcmp(tok, "<") == 0 ||
            strcmp(tok, ">") == 0 || strcmp(tok, ">>") == 0);
}

// Checks if a token is a redirection operator specifically
static bool is_redirection_op(const char *tok) {
    if (tok == NULL) return false;
    return (strcmp(tok, "<") == 0 || strcmp(tok, ">") == 0 ||
            strcmp(tok, ">>") == 0);
}

// Validates the sequence of tokens against the project's grammar rules.
static bool is_syntax_valid(char **tokens, int count) {
    if (count == 0) return true;

    // Rule: Cannot start with operators that need a preceding command.
    if (strcmp(tokens[0], "|") == 0 || strcmp(tokens[0], ";") == 0 || strcmp(tokens[0], "&") == 0) {
        return false;
    }

    // Rule: Cannot end with operators that need a subsequent command/file.
    const char *last_tok = tokens[count - 1];
    if (strcmp(last_tok, "|") == 0 || is_redirection_op(last_tok)) {
        return false;
    }

    // Rule: Check for invalid adjacent operators like `| ;` or `> |`
    for (int i = 0; i < count - 1; i++) {
        const char *tok = tokens[i];
        const char *next_tok = tokens[i+1];

        if (is_operator(tok) && !is_redirection_op(tok) && is_operator(next_tok)) {
             return false;
        }
        if (is_redirection_op(tok) && is_operator(next_tok)) {
            return false;
        }
    }
    return true;
}


// Tokenizes a raw input string into an array of strings.
int tokenize(char *input, char **tokens, int max_tokens) {
    int count = 0;
    char *p = input;
    while (*p && count < max_tokens - 1) {
        while (*p && (*p == ' ' || *p == '\t')) p++;
        if (*p == '\0') break;

        const char *op = NULL;
        if (strncmp(p, ">>", 2) == 0) op = ">>";
        else if (*p == '|') op = "|";
        else if (*p == '&') op = "&";
        else if (*p == '<') op = "<";
        else if (*p == '>') op = ">";
        else if (*p == ';') op = ";";

        if (op) {
            tokens[count++] = strndup(op, strlen(op));
            p += strlen(op);
        } else {
            char *start = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '|' && *p != '&' &&
                   *p != '<' && *p != '>' && *p != ';') p++;
            tokens[count++] = strndup(start, p - start);
        }
    }
    tokens[count] = NULL;
    return count;
}

// Parses a command string, validates it, and fills an array of command_t structs.
// Parses a command string, validates it, and fills an array of command_t structs.
int process_command(char *input, command_t *commands, int *num_commands_out) {
    char *input_copy = strdup(input);
    if (!input_copy) {
        *num_commands_out = 0;
        return 0; // Failure
    }

    char *all_tokens[MAX_TOKENS];
    int token_count = tokenize(input_copy, all_tokens, MAX_TOKENS);

    // If there are no tokens, this is an empty command.
    // This is NOT an error. It's a success with 0 commands.
    if (token_count == 0) {
        if (all_tokens[0]) free(all_tokens[0]); // Free NULL terminator if it exists
        free(input_copy);
        *num_commands_out = 0;
        return 1; // Return 1 for SUCCESS
    }
    
    if (!is_syntax_valid(all_tokens, token_count)) {
        printf("Invalid Syntax!\n");
        for (int i = 0; i < token_count; i++) free(all_tokens[i]);
        free(input_copy);
        *num_commands_out = 0;
        return 0; // Failure due to syntax
    }

    int num_commands = 0;
    int current_token_idx = 0;

    // Split tokens into command_t structs based on the '|' pipe operator.
    while (current_token_idx < token_count) {
        command_t *cmd = &commands[num_commands];
        
        // Initialize the new command struct
        memset(cmd, 0, sizeof(command_t)); // This sets num_input_files and num_output_files to 0
        cmd->argv = malloc(sizeof(char*) * MAX_TOKENS);
        int argc = 0;
        
        int command_end_idx = current_token_idx;
        while (command_end_idx < token_count && strcmp(all_tokens[command_end_idx], "|") != 0) {
            command_end_idx++;
        }

        // Parse this single command for arguments and redirections.
        // This loop populates the cmd struct.
        for (int i = current_token_idx; i < command_end_idx; i++) {
            
            if (strcmp(all_tokens[i], "<") == 0) {
                if (cmd->num_input_files < MAX_REDIRECTS) {
                    cmd->input_files[cmd->num_input_files++] = strdup(all_tokens[++i]);
                }
            } 
            
            else if (strcmp(all_tokens[i], ">") == 0) {
                if (cmd->num_output_files < MAX_REDIRECTS) {
                    cmd->output_files[cmd->num_output_files] = strdup(all_tokens[++i]);
                    cmd->output_types[cmd->num_output_files] = REDIRECT_TYPE_TRUNCATE;
                    cmd->num_output_files++;
                }
            } 
            
            else if (strcmp(all_tokens[i], ">>") == 0) {
                if (cmd->num_output_files < MAX_REDIRECTS) {
                    cmd->output_files[cmd->num_output_files] = strdup(all_tokens[++i]);
                    cmd->output_types[cmd->num_output_files] = REDIRECT_TYPE_APPEND;
                    cmd->num_output_files++;
                }
            } 
            // code from lllm starts
            // *** THIS IS THE CRITICAL FIX ***
            // Any token that is NOT a redirect op must be an argument.
            else {
                cmd->argv[argc++] = strdup(all_tokens[i]);
            }
        } // <-- The 'for' loop for parsing tokens ENDS HERE.

        // --- This logic runs AFTER the for loop ---
        cmd->argv[argc] = NULL; // NULL-terminate the argument list for this command
        num_commands++; // We have finished one complete command
        current_token_idx = command_end_idx + 1; // Move the index past the '|' for the next loop
    
    } // <-- The 'while' loop for finding commands ENDS HERE.

    // --- This logic runs AFTER all commands are parsed ---
    *num_commands_out = num_commands;
   // code from llm ends
    // Free the temporary token list created by tokenize.
    for (int i = 0; i < token_count; i++) {
        free(all_tokens[i]);
    }
    free(input_copy);

    return 1; // Success
}