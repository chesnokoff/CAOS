#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#define PAGE_SIZE 4096

volatile sig_atomic_t close_signal = 0;
volatile sig_atomic_t work_with_request = 0;
static int sock_fd;

void get_signal(int signum) {
#ifdef DEBUG
  fprintf(stderr, "Signal handler from get_signal\n");
#endif
  close_signal = 1;
  if (0 == work_with_request) {
    close(sock_fd);
    _exit(0);
  }
}

int GetFilePath(int fd, const char* path, char* file_path) {
  char file_name[FILENAME_MAX + 1] = {};
  char command[PAGE_SIZE] = {};
  if (0 > recv(fd, command, PAGE_SIZE - 1, 0)) {
    return -1;
  }
  char* end = strstr(command, "\r\n");
  if (NULL == end) {
    return -1;
  }
  size_t name_size = (end - command) - sizeof(" HTTP/1.1") - sizeof("GET ") + 2;
  size_t pos = 0;
  for (; pos < name_size; ++pos) {
    file_name[pos] = command[pos + 4];
  }
  file_name[pos] = 0;
  strcat(file_path, path);
  strcat(file_path, "/");
  strcat(file_path, file_name);
  return 0;
}

ssize_t GetSize(char* file_path) {
  struct stat st;
  if (-1 == stat(file_path, &st)) {
    perror("Calculating size of file error");
    return -1;
  }
  return st.st_size;
}

int SendSize(int fd, char* file_path) {
  ssize_t file_size = GetSize(file_path);
  if (0 > file_size) {
    return -1;
  }
  dprintf(fd, "Content-Length: %zu\r\n\r\n", file_size);
  return 0;
}

int CheckFile(int fd, char* file_path) {
  char respond_200[] = "HTTP/1.1 200 OK\r\n\r\n";
  char respond_403[] = "HTTP/1.1 403 Forbidden\r\n\r\n";
  char respond_404[] = "HTTP/1.1 404 Not Found\r\n\r\n";
  if (0 > access(file_path, F_OK)) {
    dprintf(fd, "%s", respond_404);
    return 0;
  }
  if (0 > access(file_path, R_OK)) {
    dprintf(fd, "%s", respond_403);
    return 0;
  }
  dprintf(fd, "%s", respond_200);
  if (0 > SendSize(fd, file_path)) {
    return -1;
  }
  return 1;
}

int SendFile(int fd, char* file_path) {
  int file = open(file_path, O_RDONLY);
  ssize_t file_size = GetSize(file_path);
  if (0 > file_size) {
    return -1;
  }
  if (0 > sendfile(fd, file, NULL, file_size)) {
    perror("Sending error");
    return -1;
  }
  return 0;
}

int ProcessTheRequest(int cur_client_fd, const char* path) {
  char file_path[PATH_MAX] = {};
  if (0 > GetFilePath(cur_client_fd, path, file_path)) {
    return -1;
  }
  #ifdef DEBUG
    fprintf(stderr, "file path is %s\n", file_path);
  #endif
  int file_status = CheckFile(cur_client_fd, file_path);
  if (-1 == file_status) {
    return -1;
  }
  #ifdef DEBUG
    fprintf(stderr, "Start sleeping ");
    sleep(10);
    fprintf(stderr, "End sleeping\n");
  #endif
  if (0 == file_status) {
    return 0;
  }
  if (0 > SendFile(cur_client_fd, file_path)) {
    return -1;
  }
  return 0;
}

int main(int argc, const char *argv[]) {
#ifdef DEBUG
  fprintf(stderr, "Start debug\n");
#endif
  if (argc != 3) {
    printf("Invalid arguments number\n");
    exit(1);
  }

  const char* path = argv[2];


  // SIGINT SIGTERM handler
  struct sigaction sig_action = {};
  sig_action.sa_handler = get_signal;
  sig_action.sa_flags = SA_RESTART;
  if (-1 == sigaction(SIGINT, &sig_action, NULL)){
    perror("Sigaction error");
    exit(1);
  }
  if (-1 == sigaction(SIGTERM, &sig_action, NULL)) {
    perror("Sigaction error");
    exit(1);
  }

  // Set socket
  long port = strtol(argv[1], NULL, 10);

  struct sockaddr_in address = {};
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = inet_addr("127.0.0.1");

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("socket failed");
    exit(1);
  }

  #ifdef DEBUG
  int val = 1;
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
  #endif

  if (-1 == bind(sock_fd, (const struct sockaddr*) &address, sizeof(address))) {
    perror("Binding error");
    exit(1);
  }
  if (-1 == listen(sock_fd, SOMAXCONN)) {
    perror("Listen error");
    exit(1);
  }

  while(close_signal != 1) {
    struct sockaddr_in client_addr = {};
    socklen_t addr_len;

    int client_fd;

    work_with_request = 0;
    if (0 > (client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addr_len)) && 1 != close_signal) {
    #ifdef DEBUG
      fprintf(stderr, "Signal handler after accept\n");
    #endif
      perror("Connection error");
      exit(1);
    }
    work_with_request = 1;
    if (1 == close_signal) {
      continue;
    }
    int request_result = ProcessTheRequest(client_fd, path);
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    if (-1 == request_result) {
      exit(1);
    }
  }
  close(sock_fd);
  return 0;
}