#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TYPE_SIZE sizeof(int)
#define PAGE_SIZE 1024

struct Pair{
  size_t first;
  size_t second;
};

//OK
int min(int a, int b) {
  if (a < b) {
    return a;
  }
  return b;
}

//OK
int compare_ints(const void* a, const void* b)
{
  int arg1 = *(const int*)a;
  int arg2 = *(const int*)b;
 
  if (arg1 < arg2) return -1;
  if (arg1 > arg2) return 1;
  return 0;
}

////OK
//void SwapBytes(int* x) {
//  int copy = *x;
//  *x = (copy>>24) | 
//    ((copy<<8) & 0x00FF0000) |
//    ((copy>>8) & 0x0000FF00) |
//    (copy<<24);
//}

////OK
//void ConvertBuffer(int* buffer, size_t begin, size_t size) {
//  for (size_t pos = begin; pos < begin + size; ++pos) {
//    SwapBytes(buffer + pos);
//  }
//}

// OK
int ReadIntsToBuffer(int file, int* buffer, size_t text_pos, size_t buffer_pos, size_t number) {
  lseek(file, text_pos, SEEK_SET);
  int read_bytes = read(file, buffer + buffer_pos, TYPE_SIZE * number);
  if (read_bytes < 0) {
    perror("Input error");
  }
  return read_bytes;
}

//OK
int WriteIntsFromBuffer(int file, int* buffer, size_t text_pos, size_t buffer_pos, size_t number) {
  lseek(file, text_pos, SEEK_SET);
  int write_bytes = write(file, buffer + buffer_pos, TYPE_SIZE * number);
  if (write_bytes < 0) {
    perror("Output error");
  }
  return write_bytes;
}

void Debug(int file) {
  int buffer[PAGE_SIZE] = {};
  size_t pos = 0;
  int read_bytes = ReadIntsToBuffer(file, buffer, pos, 0, PAGE_SIZE);
  while (read_bytes > 0) {
    for (size_t cur_pos = 0; cur_pos < read_bytes / TYPE_SIZE; ++cur_pos) {
      printf("%d\n", buffer[cur_pos]);
    }
    printf("end\n");
    pos += read_bytes;
    read_bytes = ReadIntsToBuffer(file, buffer, pos, 0, PAGE_SIZE);
  } 
  printf("END DEBUG\n");
}

////OK
//int ConvertFile(int file) {
//  int buffer[PAGE_SIZE] = {};
//  size_t pos = 0;
//  int read_bytes = ReadIntsToBuffer(file, buffer, pos, 0, PAGE_SIZE);
//  while (read_bytes > 0) {
//    ConvertBuffer(buffer, 0, read_bytes / TYPE_SIZE);
//    int write_bytes = WriteIntsFromBuffer(file, buffer, pos, 0, read_bytes / TYPE_SIZE);
//    if (write_bytes < 0) {
//      return -1;
//    }
//    pos += read_bytes;
//    read_bytes = ReadIntsToBuffer(file, buffer, pos, 0, PAGE_SIZE);
//  }
//  if (read_bytes < 0) {
//    return read_bytes;
//  }
//  return pos;
//}

//OK
int DivideFile(int file) {
  int buffer[PAGE_SIZE] = {};
  size_t pos = 0;
  int read_bytes = ReadIntsToBuffer(file, buffer, pos, 0, PAGE_SIZE);
  while (read_bytes > 0) {
    qsort(buffer, read_bytes / TYPE_SIZE, TYPE_SIZE, compare_ints);
    if (-1 == WriteIntsFromBuffer(file, buffer, pos, 0, read_bytes / TYPE_SIZE)) {
      return -1;
    }
    pos += read_bytes;
    read_bytes = ReadIntsToBuffer(file, buffer, pos, 0, PAGE_SIZE);
  } 
  if (read_bytes < 0) {
    return read_bytes;
  }
  return pos;
}

// OK
struct Pair Merge(int* buffer, int* buffer1, size_t number1, int* buffer2, size_t number2) {
  size_t pos = 0;
  size_t pos1 = 0;
  size_t pos2 = 0;
  while (pos1 < number1 && pos2 < number2) {
    if (buffer1[pos1] < buffer2[pos2]) {
      buffer[pos] = buffer1[pos1];
      ++pos1;
    } else {
      buffer[pos] = buffer2[pos2];
      ++pos2;
    }
    ++pos;
  }
  struct Pair pair;
  pair.first = pos1;
  pair.second = pos2;
  return(pair);
}


// OK
int Copy(int from, size_t start_from, int to, size_t start_to, size_t bytes) {
  int buffer[PAGE_SIZE] = {};
  int read_bytes = ReadIntsToBuffer(from, buffer, start_from, 0, PAGE_SIZE);
  while (read_bytes > 0 && bytes > 0) {
    if (0 > WriteIntsFromBuffer(to, buffer, start_to, 0, min(read_bytes, bytes) / TYPE_SIZE)) {
      return -1;
    }
    bytes = read_bytes > bytes ? 0 : bytes - read_bytes;
    start_from += read_bytes;
    start_to += read_bytes;
    read_bytes = ReadIntsToBuffer(from, buffer, start_from, 0, PAGE_SIZE);
  } 
  return read_bytes;
}

// OK
int SortBlocks(int file, size_t pos, size_t block_number1, size_t pos1, size_t block_number2, size_t pos2) {
  int tmp_file = open("tmp", O_RDWR | O_CREAT, 0666);
  size_t tmp_pos = 0;
  while (block_number1 > 0 && block_number2 > 0) {
    int buffer[PAGE_SIZE] = {}; 
    int buffer1[PAGE_SIZE / 2] = {};
    int buffer2[PAGE_SIZE / 2] = {};
    int read_bytes1 = ReadIntsToBuffer(file, buffer1, pos1, 0, min(block_number1, PAGE_SIZE / 2));
    if (0 > read_bytes1) {
      return read_bytes1;
    }
    int read_bytes2 = ReadIntsToBuffer(file, buffer2, pos2, 0, min(block_number2, PAGE_SIZE / 2));
    if (0 > read_bytes2) {
      return read_bytes2;
    }
    struct Pair pair = Merge(buffer, 
                             buffer1, 
                             read_bytes1 / TYPE_SIZE, 
                             buffer2, 
                             read_bytes2 / TYPE_SIZE);
    int write_bytes = WriteIntsFromBuffer(tmp_file, buffer, tmp_pos, 0, pair.first + pair.second);
    if (write_bytes < 0) {
      return write_bytes;
    }
    tmp_pos += write_bytes;
    pos1 += pair.first * TYPE_SIZE;
    pos2 += pair.second * TYPE_SIZE;

    block_number1 = block_number1 < pair.first ? 0 : block_number1 - pair.first;
    block_number2 = block_number2 < pair.second ? 0 : block_number2 - pair.second;
  }
  int read_bytes = 0;
  int write_bytes = 0;
  if (block_number1 > 0) {
    if (Copy(file, pos1, tmp_file, tmp_pos, block_number1 * TYPE_SIZE) < 0) {
      return -1;
    }
    tmp_pos += block_number1 * TYPE_SIZE;
  }
  if (block_number2 > 0) {
    if (Copy(file, pos2, tmp_file, tmp_pos, block_number2 * TYPE_SIZE) < 0) {
      return -1;
    }
    tmp_pos += block_number2 * TYPE_SIZE;
  }
  write_bytes = Copy(tmp_file, 0, file, pos, tmp_pos);
  return write_bytes;
}

int main (int argc, char** argv) {
  int file = open(argv[1], O_RDWR);
  if (file == -1) {
    perror("Input file");
    exit(1);
  }
  ssize_t total_size = DivideFile(file);
  if (0 > total_size) {
    exit(1);
  }
  size_t total_number = total_size / TYPE_SIZE;
  //Debug(file);
  for (size_t block_number = PAGE_SIZE; block_number <= total_number; block_number *= 2) {
    for (size_t pos = 0; pos / TYPE_SIZE + block_number < total_number; pos += block_number * 2 * TYPE_SIZE) {
      int block_number1 = min(block_number, total_number - pos / TYPE_SIZE);
      int block_number2 = min(block_number, total_number - pos / TYPE_SIZE - block_number);
      if (0 > SortBlocks(file, pos, block_number1, pos, block_number2, pos + block_number * TYPE_SIZE)) {
        exit(1);
      }
    }
  }
  //Debug(file);
  if (0 > total_size) {
    exit(1);
  }
  close(file);
  return 0;
}
