#include <sys/syscall.h>

#define PAGE_SIZE 4096
#define STD_IN 0
#define STD_OUT 1

extern long syscall(long number, ...);

void _start() {
  const char stack[PAGE_SIZE];
  int bytes_read = syscall(SYS_read, STD_IN, stack, PAGE_SIZE);
  while (bytes_read > 0) {
    int status = syscall(SYS_write, STD_OUT, stack, bytes_read);
    bytes_read = syscall(SYS_read, STD_IN, stack, PAGE_SIZE);
  }
  syscall(SYS_exit, 0);
}
