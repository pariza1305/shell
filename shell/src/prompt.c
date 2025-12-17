#include "prompt.h"
#include <limits.h> // For PATH_MAX
#include <pwd.h>    // For getpwuid()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For getcwd(), gethostname(), getuid()

// This static variable stores the shell's starting directory.
// It is "private" to this file and can only be accessed via the getter function.
static char shell_virtual_home[PATH_MAX];

// Initializes the shell's state.
// This function must be called once at startup to capture the initial
// working directory, which will be treated as the shell's "virtual home".
void init_shell(void) {
  if (getcwd(shell_virtual_home, sizeof(shell_virtual_home)) == NULL) {
    // This is a fatal error; the shell cannot function without this path.
    perror("Fatal: Could not get initial working directory");
    exit(1);
  }
}

// Gets the virtual home directory that was set when the shell started.
// This allows other files (like hop.c) to safely read the path.
const char* get_shell_virtual_home(void) {
    return shell_virtual_home;
}

// Displays the shell prompt in the format <Username@SystemName:current_path>.
// It correctly shortens the path with a tilde '~' if the current directory
// is inside the shell's virtual home directory.
void show_prompt(void) {
  char hostname[256];
  char current_dir[PATH_MAX];
  char display_path[PATH_MAX];
  struct passwd *pw;
  const char *username;

  // Get system hostname, with a fallback
  if (gethostname(hostname, sizeof(hostname)) == -1) {
    strncpy(hostname, "unknown_host", sizeof(hostname) - 1);
  }

  // Get username, with a fallback
  pw = getpwuid(getuid());
  username = (pw != NULL) ? pw->pw_name : "unknown_user";

  // Get the current working directory. If it fails, display a question mark.
  if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
    snprintf(display_path, sizeof(display_path), "?");
  } else {
    // Use our getter to retrieve the virtual home path.
    const char* virtual_home = get_shell_virtual_home();
    size_t home_len = strlen(virtual_home);

    if (strcmp(current_dir, virtual_home) == 0) {
      // If CWD is exactly the virtual home, display just "~".
      snprintf(display_path, sizeof(display_path), "~");
    } else if (strncmp(current_dir, virtual_home, home_len) == 0 && current_dir[home_len] == '/') {
      // If CWD is a subdirectory of virtual home, display "~/subdir".
      snprintf(display_path, sizeof(display_path), "~%s", current_dir + home_len);
    } else {
      // Otherwise, display the full absolute path.
      strncpy(display_path, current_dir, sizeof(display_path) -1);
      display_path[sizeof(display_path) - 1] = '\0';
    }
  }

  // Print the final formatted prompt and flush the output buffer.
  printf("<%s@%s:%s> ", username, hostname, display_path);
  fflush(stdout);
}
