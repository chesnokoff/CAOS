#include <sys/syscall.h>

#define PAGE_SIZE 4096
#define STD_IN 0
#define STD_OUT 1

extern long syscall(long number, ...);

void _start() {
  long heap_start = syscall(SYS_brk);
  long heap_frame_pointer = heap_start;
  long heap_size = 0;
  long heap_capacity = PAGE_SIZE;
  long break_pointer = syscall(SYS_brk, heap_frame_pointer + heap_capacity);
  long read = syscall(SYS_read, 0, heap_frame_pointer, heap_capacity - heap_size);
  char* text = 0;
  text += heap_start;
  
  while (read > 0) {
    // syscall(SYS_write, 1, heap_frame_pointer, read);
    heap_frame_pointer += read;
    heap_size += read;
    if (heap_size == heap_capacity) {
      break_pointer = syscall(SYS_brk, break_pointer + PAGE_SIZE);
      heap_capacity += PAGE_SIZE;
    }
    read = syscall(SYS_read, 0, heap_frame_pointer, heap_capacity - heap_size);
  }
  
  if (heap_size == 0 || text[heap_size - 1] != '\n') {
    if (heap_size == heap_capacity) {
      break_pointer = syscall(SYS_brk, break_pointer + 1);
      ++heap_capacity;
    }
    text[heap_size] = '\n';
    ++heap_frame_pointer;
    ++heap_size;
  }
  
  --heap_frame_pointer;
  for (long pos = heap_size - 1; pos >= 0; --pos) {
    if (text[pos] == '\n' && pos != heap_size - 1) {
      long len = heap_frame_pointer - (heap_start + pos);
      syscall(SYS_write, 1, text + pos + 1, len);
      heap_frame_pointer = heap_start + pos;
    }
  }
  syscall(SYS_write, 1, text, heap_frame_pointer - heap_start);
  text[0] = '\n';
  syscall(SYS_write, 1, text, 1);
  syscall(SYS_exit, 0);
  syscall(SYS_exit, 0);
}
