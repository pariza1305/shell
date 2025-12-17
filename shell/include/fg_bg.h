#ifndef FG_BG_H
#define FG_BG_H

void process_creation();
void fg_cmd(int argc, char **argv);
void bg_cmd(int argc, char **argv);
void jobs_cmd(int argc, char **argv);

// Wrapper functions for execute.c
void fg_command(int argc, char **argv);
void bg_command(int argc, char **argv);

#endif
