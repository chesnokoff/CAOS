#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

bool CheckOccurance(const char* text, const char* pattern, size_t pattern_size, size_t start) {
  for (size_t pos = 0; pos < pattern_size; ++pos) {
    if (text[start + pos] != pattern[pos]) {
      return false;
    }
  }
  return true;
}

void FindAndPrint(const char* text, const char* pattern, const size_t text_size) {
  size_t pattern_size = strlen(pattern);
  for (size_t pos = 0; pos <= text_size - pattern_size; ++pos) {
    if (text[pos] == pattern[0] && CheckOccurance(text, pattern, pattern_size, pos)) {
      printf("%ld\n", pos);
    }
  }
}

int main(int argc, char** argv) {
  int file = open(argv[1], O_RDWR);
  if (-1 == file) {
    perror("open");
    exit(1);
  }

  struct stat statbuf = {};
  fstat(file, &statbuf);

  char *mem = mmap(NULL,
                   statbuf.st_size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   file,
                   0);
  close(file);
  if (MAP_FAILED == mem) {
    perror("mmap");
    exit(1);
  }
  FindAndPrint(mem, argv[2], statbuf.st_size);
  munmap(mem, statbuf.st_size);
}
