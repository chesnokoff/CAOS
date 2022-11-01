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
#include <unistd.h>


void AddNewLinesInSquare(char* text, size_t square_size, size_t width) {
  for (size_t line = 0; line < square_size; ++line) {
    text[(line + 1)* square_size * width + line] = '\n';
  }
}

void NextStep(size_t* x, size_t* y, int x_dir, int y_dir) {
  if (x_dir == 1) {
    ++*x;
  }
  if (x_dir == -1) {
    --*x;
  }
  if (y_dir == 1) {
    ++*y;
  }
  if (y_dir == -1) {
    --*y;
  }
}

void MakeSpiralTurn(int* x_dir, int* y_dir) {
  if (*x_dir == 1 && *y_dir == 0) {
    *x_dir = 0;
    *y_dir = 1;
    return;
  }
  if (*x_dir == 0 && *y_dir == 1) {
    *x_dir = -1;
    *y_dir = 0;
    return;
  }
  if (*x_dir == -1 && *y_dir == 0) {
    *x_dir = 0;
    *y_dir = -1;
    return;
  }
  if (*x_dir == 0 && *y_dir == -1) {
    *x_dir = 1;
    *y_dir = 0;
    return;
  }
}

int WriteInSquare(char* text, size_t  x, size_t y, size_t square_size, size_t width, size_t value) {
  char* string_value = calloc(width + 1, 1); // zero initializing
  if (NULL == string_value) {
    perror("Memory allocation error");
    return -1;
  }
  sprintf(string_value, "%*ld", (int)width, value);
  size_t start = y * square_size * width + y + x * width;
  for (size_t pos = 0; pos < width; ++pos) {
    text[start + pos] = string_value[pos];
  }
  free(string_value);
  return 0;
}

void MakeSpiral(char* text, const size_t square_size, const size_t width) {
  size_t x = 0;
  size_t y = 0;
  int x_dir = 1;
  int y_dir = 0;
  int segment_len = square_size;
  int until_turn = segment_len;
  int changes_number = 1; 
  for (size_t number = 1; number <= square_size * square_size; ++number) {
    if (1 == until_turn) {
      MakeSpiralTurn(&x_dir, &y_dir);
      until_turn = ++changes_number % 2 == 0 ? --segment_len : segment_len;
    } else {
      --until_turn;
    }

    WriteInSquare(text, x, y, square_size, width, number);
    x += x_dir;
    y += y_dir;
  }
}

int main(int argc, char** argv) {
  size_t square_size = 0;
  size_t width = 0;
  if (1 != sscanf(argv[2], "%zu", &square_size) || 1 != sscanf(argv[3], "%zu", &width)) {
    perror("Conversation error");
    exit(1);
  }
  int file = open(argv[1], O_RDWR | O_CREAT, 0666);
  if (-1 == file) {
    perror("open");
    exit(1);
  }
  off_t file_size = square_size * square_size * width + square_size;
  if (0 > ftruncate(file, file_size)) {
    perror("File creating error");
    exit(1);
  }

  char* mem = mmap(NULL,
                   file_size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   file,
                   0);
  close(file);
  if (MAP_FAILED == mem) {
    perror("mmap");
    exit(1);
  }

  AddNewLinesInSquare(mem, square_size, width);
  MakeSpiral(mem, square_size, width);
  munmap(mem, file_size);
}
