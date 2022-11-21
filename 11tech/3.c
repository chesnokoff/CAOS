#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
pid_t launch(const char *cmd, int fd_in, int fd_out) {
  pid_t pid = fork();

  if (0 == pid) {
    if (fd_in != STDIN_FILENO) {
      if (-1 == dup2(fd_in, STDIN_FILENO)) {
        return -1;
      }
      close(fd_in);
    }
    
    if (fd_out != STDOUT_FILENO) {
      if(-1 == dup2(fd_out, STDOUT_FILENO)) {
        return -1;
      }
      close(fd_out);
    }

    execlp(cmd, cmd, NULL);
    return -1;
  }

  return pid;
}


int main(int argc, const char *argv[]) {
  if (argc == 1) {
      return 0;
  }
  if (argc == 2) {
    pid_t pid = launch(argv[1], STDIN_FILENO, STDOUT_FILENO);
    if (-1 == pid) {
      perror("launch");
      exit(1);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
      _exit(1);
    }
    return 0;
  }
  int* pipe_fd = calloc((argc - 2) * 2, sizeof(int));
  for (size_t pos = 0; pos < (argc - 2) * 2; pos += 2) {
    pipe(pipe_fd + pos);
  }
  for (size_t cur_command = 1; cur_command < argc; ++cur_command) {
    int in = pipe_fd[(cur_command - 1) * 2 - 2];
    if (1 == cur_command) {
      in = 0;
    }
    int out = pipe_fd[(cur_command - 1) * 2 + 1];
    if (argc - 1 == cur_command) {
      out = 1;
    }
    pid_t pid = launch(argv[cur_command], in, out);
    if (-1 == pid) {
      perror("launch");
    }
    int status = 0;
    waitpid(pid, &status, 0);
    if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
      free(pipe_fd);
      _exit(1);
    }
    if (in != STDIN_FILENO) {
      close(in);
    }
    if (out != STDOUT_FILENO) {
      close(out);
    }
  }
  return 0;
}
