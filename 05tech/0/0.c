#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char** argv) {
  const int Size = 4090;
  int input_file = open(argv[1], O_RDONLY);
  if (input_file == -1) {
    perror("Input file");
    exit(1);
  }
  int digit_output = open(argv[2], O_WRONLY | O_CREAT, 0666);
  if (digit_output == -1) {
    perror("Digit output file");
    exit(2);
  }
  int rest_output= open(argv[3], O_WRONLY | O_CREAT, 0666);
  if (rest_output == -1) {
    perror("Non-digit output file");
    exit(2);
  }
  char buf[4090] = {};
  char digit_buf[4090] = {};
  char rest_buf[4090] = {};
  size_t digit_size = 0;
  size_t rest_size = 0;
  int read_bytes = read(input_file, buf, Size);
  if (read_bytes == -1) {
    perror("Read input file");
    exit(3);
  }
  while (read_bytes > 0) {
    for (size_t pos = 0; pos < read_bytes; ++pos) {
      if (isdigit(buf[pos]) == 0) {
        rest_buf[rest_size] = buf[pos];
        ++rest_size;
        continue;
      }
      digit_buf[digit_size] = buf[pos];
      ++digit_size;
    }
    int write_bytes = write(digit_output, digit_buf, digit_size);
    if (write_bytes == -1) {
      perror("Write digits");
      exit(3);
    }
    write_bytes = write(rest_output, rest_buf, rest_size);
    if (write_bytes == -1) {
      perror("Write non-digits");
      exit(3);
    }
    digit_size = 0;
    rest_size = 0;
    read_bytes = read(input_file, buf, Size);
    if (read_bytes == -1) {
      perror("Read input file");
      exit(3);
    }
  }
  close(input_file);
  close(digit_output);
  close(rest_output);
  return 0;
}
