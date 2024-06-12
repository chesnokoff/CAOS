#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>

#define BUF_SZ 4096
#define EPOLL_QUEUE 64

size_t read_data_and_count(size_t N, int in[]) {
  if (0 == N) return 0;

  char data[BUF_SZ];
  struct epoll_event events[EPOLL_QUEUE];

  int epfd = epoll_create(N);
  if (0 > epfd) {
    perror("Creating epoll fd error");
    exit(1);
  }

  for (int i = 0; i < N; i++) {
    int fd_flags = fcntl(in[i], F_GETFL);
    if (0 > fd_flags) {
      perror("Getting flags error");
      exit(1);
    }
    fd_flags |= O_NONBLOCK;
    if (0 > fcntl(in[i], F_SETFL, fd_flags)) {
      perror("Setting flags error");
      exit(1);
    }

    events[0].data.fd = in[i];
    events[0].events = EPOLLIN;
    if (0 > epoll_ctl(epfd, EPOLL_CTL_ADD, in[i], events)) {
      perror("Events setting error");
      exit(1);
    }
  }

  size_t total = 0;
  int closed = 0;

  while (closed < N) {
    int evcount = epoll_wait(epfd, events, EPOLL_QUEUE, -1);
    if (0 > evcount) {
      perror("Waiting error");
      exit(1);
    }
    for (int i = 0; i < evcount; i++) {
      int read_b = 0;
      while(read_b = read(events[i].data.fd, data, BUF_SZ)) {
        if (-1 == read_b) {
          if (errno == EWOULDBLOCK) {
            break;
          } else {
            perror("read");
            exit(1);
          }
        }

        total += read_b;
      }

      if(!read_b) {
        closed++;
        close(events[i].data.fd);
      }
    }
  }

  return total;
}
