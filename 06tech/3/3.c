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

// OK
int FileExists(char* file_name) {
  struct stat file_stat;
  if (lstat(file_name, &file_stat) == 0) {
    return 1;
  }
  return 0;
}

// OK
int IsRegular(char* file_name) {
  struct stat file_stat;
  if (lstat(file_name, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
    return 1;
  }
  return 0;
}

// OK
int IsSymLink(char* file_name) {
  struct stat file_stat;
  if (lstat(file_name, &file_stat) == 0 && S_ISLNK(file_stat.st_mode)) {
    return 1;
  }
  return 0;
}

// OK
int PrintFileUnderSymLink(char* file_name) {
  char real_path[PATH_MAX] = {};
  if (NULL == realpath(file_name, real_path)) {
    return 0;
  }
  int write_bytes = strlen(real_path);
  real_path[write_bytes++] = '\n';
  write(1, real_path, write_bytes);
  return 1;
}

// OK
void GetName(char* path, char* dest) {
  char* occurrence_pointer = strrchr(path, '/');
  if (NULL == occurrence_pointer) {
    strcpy(dest, path);
    return;
  }
  strcpy(dest, occurrence_pointer + 1);
}

int main() {
  char file_name[PATH_MAX] = {}; // create buffer for getline

  while (fgets(file_name, sizeof(file_name), stdin)) {
    size_t len = strlen(file_name);
    if (file_name[len - 1] == '\n') {
      file_name[len - 1] = 0;
    }
    int exists = FileExists(file_name);
    if (exists == 0) {
      continue;
    }
    int is_link = IsSymLink(file_name);
    if (is_link == 1) {
      PrintFileUnderSymLink(file_name);
      continue;
    }

    int is_regular = IsRegular(file_name);
    if (is_regular == 0) {
      continue;
    }
    char link[PATH_MAX] = "link_to_";
    char name[PATH_MAX] = {};
    GetName(file_name, name);
    strcat(link, name);
    symlink(file_name, link);
  }
  return 0;
}
