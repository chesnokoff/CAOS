#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Item {
  int value;
  uint32_t next_pointer;
};

void Iterate(struct Item* item_array) {
  uint32_t pos = 0;
  do {
    printf("%d\n", item_array[pos].value);
    pos = item_array[pos].next_pointer / sizeof(struct Item);
  } while (pos != 0);
}

int main(int argc, char** argv) {
  int file = open(argv[1], O_RDWR);
  if (-1 == file) {
    perror("open");
    exit(1);
  }

  struct stat statbuf = {};
  fstat(file, &statbuf);

  char* mem = mmap(NULL,
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
  if (statbuf.st_size < sizeof(struct Item)) {
    return 0;
  }
  struct Item* item_arr = (struct Item*) mem;
  Iterate(item_arr);
  munmap(mem, statbuf.st_size);
}
