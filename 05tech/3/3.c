#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>

struct Item
{
  INT value;
  DWORD next_pointer;
};


int main (int argc, char** argv) {
  HANDLE input_file = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == input_file) {
    printf("Input error");
    exit(1);
  }
  struct Item item;
  unsigned long read_bytes = 0;
  BOOL read_file = ReadFile(input_file, &item, sizeof(item), &read_bytes, NULL);
  while (read_file == TRUE && read_bytes >= sizeof(item)) {
    int print_chars = printf("%d\n", item.value);
    if (print_chars < 0) {
      perror("Output");
      exit(1);
    }
    if (item.next_pointer == 0) {
      break;
    }
    LARGE_INTEGER next;
    next.QuadPart = item.next_pointer;
    BOOL set = SetFilePointerEx(input_file, next, NULL, FILE_BEGIN);
    if (set == FALSE) {
      printf("Error input");
      exit(1);
    }
    read_file = ReadFile(input_file, &item, sizeof(item), &read_bytes, NULL);
  }
  if (read_file == FALSE) {
    perror("Input file");
    exit(1);
  }
  CloseHandle(input_file);
  return 0;
}