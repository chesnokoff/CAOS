#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void* read_and_add(void* arg) {
  int* sum = (int*) arg;
  int number = 0;

#ifdef DEBUG
  fprintf(stderr, "Start reading\n");
#endif
  while ((scanf("%d", &number)) > 0) {
    *sum += number;
  }
#ifdef DEBUG
  fprintf(stderr, "End reading\n");
#endif
  return NULL;
}

int main(int argc, const char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Invalid arguments number");
    exit(1);
  }

  long N = strtol(argv[1], NULL, 10);

  pthread_attr_t pthread_attr = {};
  pthread_attr_init(&pthread_attr);
  pthread_attr_setstacksize(&pthread_attr, 4096);
#ifdef DEBUG
  fprintf(stderr, "Attribute was set\n");
#endif
  pthread_t* threads = calloc(N, sizeof(pthread_t));
  if (NULL == threads) {
    perror("Allocation error");
    exit(1);
  }
  int* sums = calloc(N, sizeof(int));
  if (NULL == sums) {
    perror("Allocation error");
    exit(1);
  }
#ifdef DEBUG
  fprintf(stderr, "Start execution\n");
#endif

  for (size_t i = 0; i < N; i++) {
    if (0 > pthread_create(threads + i, &pthread_attr, read_and_add, sums + i)) {
      perror("Creating new thread error");
      exit(1);
    }
  }

  int answer = 0;
  for (int i = 0; i < N; i++) {
    if (0 > pthread_join(threads[i], NULL)) {
      perror("Waiting thread error");
      exit(1);
    }
  }

  for (int i = 0; i < N; i++) {
    answer += sums[i];
  }

  printf("%d\n", answer);

#ifdef DEBUG
  fprintf(stderr, "Done\n");
#endif

  pthread_attr_destroy(&pthread_attr);
  free(sums);
  free(threads);
  return 0;
}