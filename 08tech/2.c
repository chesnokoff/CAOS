#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <stdint.h>
#include <sys/wait.h>

int main() {
  char word[4096] = {};
  int counter = 0;
  int level = 0;
  while(1 == scanf("%s", word)) {
    ++counter;
    pid_t pid = fork();

    if (-1 == pid) {
      perror("Couldn't create new process");
      break;
    } 
    
    if (0 == pid) {
      ++level;
      continue;
    } else {
      int exit_status = 0;
      waitpid(pid, &exit_status, 0);
      if (WIFEXITED(exit_status)) {
        exit_status = WEXITSTATUS(exit_status);
        counter = exit_status;
        if (0 == level) {
          break;
        }
        _exit(exit_status);
      }
      else break;
    }
  }
  if (level > 0) {
    _exit(counter);
  }
  printf("%d\n", counter);
}

