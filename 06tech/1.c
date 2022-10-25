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

int FileExists(char* file_name) {
  struct stat file_stat;
  if (stat(file_name, &file_stat) == 0) {
    return 1;
  }
  return 0;
}

int IsExecutable(char* file_name) {
  struct stat file_stat;
  if (stat(file_name, &file_stat) == 0 && file_stat.st_mode & S_IXUSR) {
    return 1;
  }
  return 0;
}

int CheckInterpret(char* file_name) {
  int file = open(file_name, O_RDONLY);
  char buffer [PATH_MAX] = {};
  if (read(file, buffer, PATH_MAX) < 0) {
    perror("Read file error");
    close(file);
    return -1;
  }
  if (buffer[0] != '#' || buffer[1] != '!') {
    close(file);
    return 0;
  }
  for (size_t pos = 0; pos < PATH_MAX; ++pos) {
    buffer[pos] = buffer[pos + 2];
    if (buffer[pos] == '\n' || buffer[pos] == '\0' || buffer[pos] == ' ') {
      buffer[pos] = '\0';
      buffer[pos + 1] = '\0';
      buffer[pos + 2] = '\0';
      break;
    }
  }
  struct stat file_stat;
  if (stat(buffer, &file_stat) == 0 && file_stat.st_mode & S_IXUSR) {
    close(file);
    return 1;
  }
  close(file);
  return 0;
}

int CheckELF(char* file_name) {
  int file = open(file_name, O_RDONLY);
  char buffer[4] = {};
  if (read(file, buffer, 4) < 0) {
    perror("Read file error");
    close(file);
    return -1;
  }
  if (buffer[0] != 0x7F || buffer[1] != 0x45 || buffer[2] != 0x4c || buffer[3] != 0x46) {
    close(file);
    return 0;
  }
  close(file);
  return 1;
}

int main() {
  size_t buffer_size = PATH_MAX;
  char* file_name = (char *)malloc(PATH_MAX); // create buffer for getline
  if (file_name == NULL) {
    perror("Memory allocation error");
    exit(1);
  }

  int read_bytes = 0;

  while ((read_bytes = getline(&file_name, &buffer_size, stdin)) >= 0) {
    if (feof(stdin)) { // Check end of input
      break;
    }

    file_name[read_bytes - 1] = '\0'; // Remove new line symbol
    
    int exists = FileExists(file_name);
    if (exists == 0) {
      continue;
    }
    
    int is_executable = IsExecutable(file_name);
    if (is_executable == 0) {
      continue;
    }
    
    int has_interpret = CheckInterpret(file_name);
    if (has_interpret < 0) {
      exit(1);
    }
    if (has_interpret == 1) {
      continue;
    }
    
    int has_ELF = CheckELF(file_name);
    if (has_ELF < 0) {
      exit(1);
    }
    if (has_ELF == 1) {
      continue;
    }
    printf("%s\n", file_name);
  }
}
