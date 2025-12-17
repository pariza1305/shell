#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h" 

int is_builtin(const char *cmd);

void execute_builtin(command_t *cmd);

#endif // BUILTINS_H