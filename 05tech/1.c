#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Item
{
  int value;
  uint32_t next_pointer;
};


int main (int argc, char** argv) {
  int input_file = open(argv[1], O_RDONLY);
  if (input_file == -1) {
    perror("Input file");
    exit(1);
  }
  struct Item item;
  int read_bytes = read(input_file, &item, sizeof(item));
  while (read_bytes > 0) {
    int print_chars = printf("%d", item.value);
    if (print_chars < 0) {
      perror("Output");
      exit(1);
    }
    print_chars = printf("%c", '\n');
    if (print_chars < 0) {
      perror("Output");
      exit(1);
    }
    if (item.next_pointer == 0) {
      break;
    }
    lseek(input_file, item.next_pointer, SEEK_SET);
    read_bytes = read(input_file, &item, sizeof(item));
  }
  if (read_bytes == -1) {
    perror("Input file");
    exit(1);
  }
  return 0;
}
