#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <stdint.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
  size_t proc_cnt = 1;
  const int total = atoi(argv[1]);
  int cur = total;

  while(1) {
    if (cur == 0) {
      break;
    }

    pid_t pid = fork();

    if (-1 == pid) {
      printf("%ld\n", proc_cnt);
      break;
    } 
    
    if (0 == pid) {
      --cur;
      continue;
    } else {
      waitpid(pid, NULL, 0);
      if (total == cur) {
        printf("%d\n", cur);
      } else {
        printf("%d ", cur);
      }
      break;
    }
  }
}

