#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t exit_command = 0;
volatile sig_atomic_t file_number = 0;

void process_interceptor(int signum) {
  file_number = signum - SIGRTMIN;
  if (0 == file_number) {
    exit_command = 1;
  }
}

int main(int argc, char** argv) {
  FILE** files = (FILE**)malloc(argc * sizeof(FILE*));
  if (files == NULL) {
    perror("Memory not allocated.\n");
    exit(1);
  }
  for (size_t pos = 1; pos < argc; ++pos) {
    files[pos] = fopen(argv[pos], "r");
  }
  struct sigaction sig_action = {};
  sig_action.sa_handler = process_interceptor;
  sig_action.sa_flags = SA_RESTART;
  for (size_t pos = SIGRTMIN; pos <=SIGRTMAX; ++pos) {
    sigaction(pos, &sig_action, NULL);
  }
  while (1) {
    if(file_number > 0) {
      char* line = NULL;
      size_t len = 0;
      ssize_t read = 0;
      if ((read = getline(&line, &len, files[file_number])) > 0) {
        printf("%s", line);
        fflush(stdout);
      }
      file_number = 0;
      if(line) {
        free(line);
      }
    }

    if(exit_command) {
      break;
    }

    pause();
  }
  for (size_t pos = 1; pos < argc; ++pos) {
    fclose(files[pos]);
  }
  free(files);
  return 0;
}
