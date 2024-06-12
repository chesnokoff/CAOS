#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>

int main() {
  size_t buffer_size = PATH_MAX;
  char* file_name = (char *)malloc(buffer_size * sizeof(char));
  if (file_name == NULL) {
    perror("Unable to allocate buffer");
    exit(1);
  }
  off_t total_size = 0;
  ssize_t read_bytes = 0;
  while ((read_bytes = getline(&file_name, &buffer_size, stdin)) >= 0) {
    if (feof(stdin)) {
      break;
    }
    file_name[read_bytes - 1] = '\0';
    struct stat file;
    lstat(file_name, &file);
    if (S_ISREG(file.st_mode)) {
      total_size += file.st_size;
    }
  }
  printf("%ld", total_size);
}
