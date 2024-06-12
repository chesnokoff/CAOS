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
  if (argc != 3) {
    exit(1);
  }
  int pipe_fd[2];
  pipe(pipe_fd);

  pid_t pid1 = launch(argv[1], STDIN_FILENO, pipe_fd[1]);
  if (-1 == pid1) {
    perror("launch");
  }
  close(pipe_fd[1]);
  pid_t pid2 = launch(argv[2], pipe_fd[0], STDOUT_FILENO);
  if (-1 == pid2) {
    perror("launch");
  }
  close(pipe_fd[0]);
  int status = 0;
  waitpid(-1, &status, 0);
  if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
    _exit(1);
  }
  waitpid(-1, &status, 0);
  if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
    _exit(1);
  }
}