# Custom C Shell

A feature-rich, interactive command-line shell implemented in C. This project simulates a standard UNIX shell (like bash or zsh) by providing its own Read-Eval-Print-Loop (REPL), custom built-in commands, job control, piping, and I/O redirection.

## Features

- **Interactive Prompt:** Displays a dynamically updated prompt.
- **Command Execution:** Run standard system commands and executables. Supports executing multiple commands sequentially using `;`.
- **Background Processes:** Execute processes in the background using `&`.
- **Custom Built-in Commands:**
  - `hop`: A custom implementation of `cd` for changing the current working directory.
  - `reveal`: A custom implementation of `ls` for listing files and directories.
  - `log`: A command history system that stores and retrieves past commands.
  - `ping`: Send signals to processes (Usage: `ping <pid> <signal_no>`).
  - `activities`: List all current running or stopped processes spawned by the shell.
  - `fg` / `bg`: Bring a background job to the foreground or resume a stopped background job.
  - `exit`: Cleanly exit the shell.
- **Input/Output Redirection:** Route inputs and outputs using `<`, `>`, and `>>`.
- **Command Piping:** Chain multiple commands together using the `|` operator (e.g., `cmd1 | cmd2 | cmd3`).
- **Signal Handling:** Gracefully handles keyboard interrupts and signals:
  - `Ctrl+C` (SIGINT): Interrupts any running foreground process.
  - `Ctrl+Z` (SIGTSTP): Stops the currently running foreground process and pushes it to the background.
  - `Ctrl+D` (EOF): Logs out and exits the shell.

## Directory Structure

The project is organized in a modular structure to separate concerns and ensure maintainability:

```
.
├── Makefile          # Build configuration file
├── include/          # Header files (.h) with function declarations
│   ├── activities.h
│   ├── background.h
│   ├── builtins.h
│   ├── cmd_piping.h
│   ├── ctrlcdz.h
│   ├── execute.h
│   ├── exit.h
│   ├── fg_bg.h
│   ├── hop.h
│   ├── input.h
│   ├── input_redirect.h
│   ├── log.h
│   ├── output_redirect.h
│   ├── parser.h
│   ├── ping.h
│   ├── prompt.h
│   ├── reveal.h
│   └── utils.h
└── src/              # Source code files (.c) with implementations
    ├── activities.c  # Tracks active processes and states
    ├── background.c  # Handles spawning and monitoring background tasks
    ├── builtins.c    # Standard built-in shell commands
    ├── cmd_piping.c  # Implements inter-process communication via pipes
    ├── ctrlcdz.c     # Signal handlers (Ctrl+C, Ctrl+Z)
    ├── execute.c     # Core logic for executing commands
    ├── exit.c        # Shell cleanup and exit procedures
    ├── fg_bg.c       # Job control (fg/bg)
    ├── hop.c         # `hop` command implementation
    ├── input.c       # Input handling
    ├── input_redirect.c
    ├── output_redirect.c
    ├── log.c         # History / log system
    ├── main.c        # Shell entry point and main REPL loop
    ├── parser.c      # Parses raw input into command arguments and operators
    ├── ping.c        # Custom signal sending command
    ├── prompt.c      # Displays the user prompt
    ├── reveal.c      # `reveal` command implementation
    └── utils.c       # Helper functions
```

## Getting Started

### Prerequisites

- A C compiler (e.g., `gcc`)
- `make` utility
- A POSIX-compliant operating system (Linux, macOS, etc.)

### Building the Shell

Navigate to the `shell` directory where the `Makefile` is located and run:

```bash
cd shell
make
```

This will compile the source code and generate an executable named `shell.out`.

### Running the Shell

After a successful build, start the shell by running:

```bash
./shell.out
```

### Cleaning Up

To remove compiled object files and the executable, run:

```bash
make clean
```
