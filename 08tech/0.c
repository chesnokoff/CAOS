#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <stdint.h>
#include <sys/wait.h>

int main() {
  size_t proc_cnt = 1;
  
  while(1) {
    pid_t pid = fork();

    if (-1 == pid) {
      printf("%ld\n", proc_cnt);
      break;
    } 
    
    if (0 == pid) {
      proc_cnt += 1;
      continue;
    }
    
    waitpid(pid, NULL, 0);
    break;
  
  }
}

