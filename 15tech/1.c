#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <bits/types/sig_atomic_t.h>

volatile sig_atomic_t answer = 0;

void* read_and_add(void* arg) {
  int number = 0;

#ifdef DEBUG
  fprintf(stderr, "Start reading\n");
#endif
  while ((scanf("%d", &number)) > 0) {
#ifdef DEBUG
    fprintf(stderr, "Read: %d\n", number);
#endif
    answer += number;
  }
  return NULL;
}

int main(int argc, const char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Invalid arguments number");
    exit(1);
  }

  long N = strtol(argv[1], NULL, 10);

  pthread_t* threads = calloc(N, sizeof(pthread_t));
  if (NULL == threads) {
    perror("Allocation error");
    exit(1);
  }
#ifdef DEBUG
  fprintf(stderr, "Start execution\n");
#endif

  for (size_t i = 0; i < N; i++) {
    if (0 > pthread_create(threads + i, NULL, read_and_add, NULL)) {
      perror("Creating new thread error");
      exit(1);
    }
  }
  for (int i = 0; i < N; i++) {
    if (0 > pthread_join(threads[i], NULL)) {
      perror("Waiting thread error");
      exit(1);
    }
  }

  printf("%d\n", answer);

#ifdef DEBUG
  fprintf(stderr, "Done\n");
#endif

  free(threads);
  return 0;
}