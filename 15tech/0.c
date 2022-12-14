#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct data{
  pthread_mutex_t* mutex;
  int N;
  double number;
  double* left;
  double* right;
} data_t;

void* work_with_data(void* arg) {
  data_t* current_data = arg;

  pthread_mutex_lock(current_data->mutex);

  for (int iteration = 0; iteration < current_data->N; ++iteration) {
    *current_data->left += 0.99;
    current_data->number += 1.0;
    *current_data->right += 1.01;
#ifdef DEBUG
    fprintf(stderr, "Cur number is: %g\n", current_data->number);
#endif
  }
  pthread_mutex_unlock(current_data->mutex);
  return NULL;

}

void InitData(data_t* data_base, int N, int k, pthread_mutex_t* mutex) {
  for (int pos = 0; pos < k; ++pos) {
    int left = pos - 1;
    int right = pos + 1;
    if (0 == pos) {
      left = k - 1;
    }
    if (k - 1 == pos) {
      right = 0;
    }
    data_base[pos].mutex = mutex;
    data_base[pos].number = 0;
    data_base[pos].left = &data_base[left].number;
    data_base[pos].right = &data_base[right].number;
    data_base[pos].N = N;
  }
}

int main(int argc, const char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Invalid arguments number");
    exit(1);
  }

  int N = strtol(argv[1], NULL, 10);
  int k = strtol(argv[2], NULL, 10);

  pthread_t* threads = calloc(k, sizeof(pthread_t));
  data_t* data_base = calloc(k, sizeof(data_t));
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  InitData(data_base, N, k, &mutex);

#ifdef DEBUG
  fprintf(stderr, "Start execution\n");
#endif

  for (size_t i = 0; i < k; i++) {
    if (0 > pthread_create(threads + i, NULL, work_with_data, (void *)(data_base + i))) {
      perror("Creating new thread error");
      exit(1);
    }
  }

  for (int i = 0; i < k; i++) {
    if (0 > pthread_join(threads[i], NULL)) {
      perror("Waiting thread error");
      exit(1);
    }
  }

  if (0 > pthread_mutex_destroy(&mutex)) {
    perror("Destroying mutex error");
    exit(1);
  }

  for (int i = 0; i < k; i++) {
    printf("%.10g ", data_base[i].number);
  }

  printf("\n");

#ifdef DEBUG
  fprintf(stderr, "Done\n");
#endif


  free(threads);
  free(data_base);

  return 0;

}