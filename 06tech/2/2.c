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
#include <string.h>
#include <linux/limits.h>

char* get_to_prev_dir(char* path, char* pos) {
  if (pos != path && *(pos - 1) == '/') {
    --pos;
  }
  while (pos != path && *(pos - 1) != '/') {
    --pos;
  }
  return pos;
}

void make_zero(char* str, char* begin, char* end) {
  char* copy_begin = begin;
  for (; copy_begin != end; ++copy_begin) {
    *copy_begin = '\0';
  }
}

char* copy(char* str, char* dest, char* begin, char* end) {
  char* copy_dest = dest;
  char* copy_begin = begin;
  while (copy_begin != end) {
    *(copy_dest++) = *(copy_begin++);
  }
  return copy_dest;
}

char* until_next(char* str, char* pos, char symbol) {
  while (*pos != symbol && *pos != 0) {
    ++pos;
  }
  return pos;
}

extern void normalize_path(char* path) {
  char* current = path; // Current file name pos
  char* it = path;      // Helper pointer
  char* start = path;
  // while (path[it] != '/'&& path[it] != '\0') {
  //   path[current++] = path[it++];
  // }

  if (*it == '\0') {
    make_zero(path, current, it);
    return;
  }
  
  if (*it == '/') {
    *(current++) = *(it++);
  }

  while (1) {
    if (*it == '/') {
      ++it;
      continue;
    }
    if (*it == 0) {
      make_zero(path, current, it);
      return;
    }
    if (*it == '.') {
      if (it[1] == '.' && it[2] == '/') {
        current = get_to_prev_dir(path, current);
        it = until_next(path, it, '/');
        continue;
      }
      if (it[1] == '/') {
        it = until_next(path, it, '/');
        continue;
      }
    }
    start = it;
    it = until_next(path, it, '/');
    current = copy(path, current, start, ++it);
  }
}
