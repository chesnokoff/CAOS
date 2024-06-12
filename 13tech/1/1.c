#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define PAGE_SIZE 4096
#define MAX_CLIENTS 1024


volatile sig_atomic_t close_signal = 0;
static int listener_fd;
static int epoll_fd;

void get_signal(int signum) {
#ifdef DEBUG
  fprintf(stderr, "Signal handler from get_signal\n");
#endif
  close_signal = 1;
}

int ProcessTheRequest(int cur_client_fd) {
  char buffer[PAGE_SIZE] = {};
  int read_bytes = 0;
  read_bytes = read(cur_client_fd, buffer, PAGE_SIZE);
  if (0 >  read_bytes && close_signal != 1) {
    if (errno != EWOULDBLOCK) {
      perror("read");
      return -1;
    }
  }
  for (size_t pos = 0; pos < read_bytes; ++pos) {
    buffer[pos] = toupper(buffer[pos]);
  }
  if (0 > write(cur_client_fd, buffer, read_bytes)) {
    perror("write");
    return -1;
  }
  return 0;
}

int SetErrorHandlers() {
  struct sigaction sig_action = {};
  sig_action.sa_handler = get_signal;
  sig_action.sa_flags = SA_RESTART;
  if (-1 == sigaction(SIGTERM, &sig_action, NULL)) {
    perror("Sigaction error");
    return -1;
  }
#ifdef DEBUG
  if (-1 == sigaction(SIGINT, &sig_action, NULL)) {
    perror("Sigaction error");
    return -1;
  }
#endif
  return 0;
}

int SetListener(long port) {
  struct sockaddr_in address = {};
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = inet_addr("127.0.0.1");

  listener_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (listener_fd < 0) {
    perror("socket failed");
    return -1;
  }

#ifdef DEBUG
  int val = 1;
  setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  setsockopt(listener_fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
#endif

  if (-1 == bind(listener_fd, (const struct sockaddr*) &address, sizeof(address))) {
    perror("Binding error");
    return -1;
  }
  if (-1 == listen(listener_fd, SOMAXCONN)) {
    perror("Listen error");
    return -1;
  }
  return 0;
}

int SetEpollFd() {
  epoll_fd = epoll_create(MAX_CLIENTS);
  if (0 > epoll_fd) {
    perror("Creating epoll fd error");
    return -1;
  }
  struct epoll_event listener_event = {};
  listener_event.data.fd = listener_fd;
  listener_event.events = EPOLLIN;
  if (0 > epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listener_fd, &listener_event)) {
    perror("Events setting error");
    return -1;
  }
  return 0;
}

int CreateNewConnection() {
  struct sockaddr_in client_addr = {};
  socklen_t addr_len;

  int client_fd;

  if (0 > (client_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addr_len)) && 1 != close_signal) {
    perror("Connection error");
    return -1;
  }
  struct epoll_event event = {};
  event.data.fd = client_fd;
  event.events = EPOLLIN | EPOLLRDHUP;
  if (0 > epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event)) {
    perror("Events setting error");
    return -1;
  }
  return client_fd;
}

void RemoveClientFromList(int* clients, int client, size_t clients_number) {
  size_t pos = 0;
  for (; pos < clients_number; ++pos) {
    if (client == clients[pos]) {
      break;
    }
  }
  if (pos == clients_number) {
    return;
  }
  for (; pos < clients_number - 1; ++pos) {
    clients[pos] = clients[pos + 1];
  }
  clients[pos] = -1;
}

int ShutDownClients(int* clients, size_t number) {
  for (size_t pos = 0; pos < number; ++pos) {
    if (0 > clients[pos]) {
      continue;
    }
    if (0 > shutdown(clients[pos], SHUT_RDWR)) {
      return -1;
    }
  }
  return 0;
}

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    printf("Invalid arguments number\n");
    exit(1);
  }

  int clients[MAX_CLIENTS] = {};
  size_t clients_number = 0;
  for (size_t pos = 0; pos < MAX_CLIENTS; ++pos) {
    clients[pos] = -1;
  }

  // SIGINT SIGTERM handler
  if (0 > SetErrorHandlers()) {
    exit(1);
  }

  // Set socket
  long port = strtol(argv[1], NULL, 10);
  if (0 > SetListener(port)) {
    exit(1);
  }

  // Set epoll
  if (0 > SetEpollFd()) {
    exit(1);
  }

  struct epoll_event* events = calloc(MAX_CLIENTS, sizeof(struct epoll_event));
  if (events == NULL) {
    perror("Allocation error");
  }

  while(close_signal != 1) {
    int events_count = epoll_wait(epoll_fd, events, MAX_CLIENTS, 1000);
    if (0 > events_count && 1 != close_signal) {
      perror("Waiting error");
      exit(1);
    }
    if (1 == close_signal) {
      continue;
    }
    for (size_t cur_event = 0; cur_event < events_count; ++cur_event) {
      if (events[cur_event].data.fd == listener_fd) {
        int new_client = CreateNewConnection();
        if (0 > new_client) {
          exit(1);
        }
        clients[clients_number] = new_client;
        ++clients_number;
        continue;
      }
      if (EPOLLRDHUP & events[cur_event].events) {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[cur_event].data.fd, NULL);
        close(events[cur_event].data.fd);
        RemoveClientFromList(clients, events[cur_event].data.fd, clients_number);
        --clients_number;
        continue;
      }
      if (EPOLLIN & events[cur_event].events) {
        if (0 > ProcessTheRequest(events[cur_event].data.fd)) {
          exit(1);
        }
      }
    }
  }

  if (0 > ShutDownClients(clients, clients_number)) {
    exit(1);
  }
  close(epoll_fd);
  free(events);
  return 0;
}