#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    exit(1);
  }

  long port = strtol(argv[2], NULL, 10);

  struct sockaddr_in address = {};

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  inet_pton(AF_INET, argv[1], &address.sin_addr);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == connect(sockfd, (struct sockaddr*) &address, sizeof(address))) {
    perror("Problem with connection");
    exit(1);
  }

  int query = 0;
  int res = 0;

  while (EOF != scanf("%d", &query)) {
    write(sockfd, &query, sizeof(query));
    int read_status = read(sockfd, &res, sizeof(res));

    if (0 == read_status) {
      break;
    }
    printf("%d\n", res);
  }

  shutdown(sockfd, SHUT_RDWR); 
  close(sockfd);
  return 0;
}
