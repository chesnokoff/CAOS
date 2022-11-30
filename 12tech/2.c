#include <signal.h>
#include <unistd.h>
#include <stdio.h>

volatile sig_atomic_t exit_command = 0;
volatile sig_atomic_t sigusr1 = 0;
volatile sig_atomic_t sigusr2 = 0;

void process_sigint_sigterm(int signum) {
  exit_command = 1;
}

void process_sigusr1(int signum) {
  sigusr1 = 1;
}

void process_sigusr2(int signum) {
  sigusr2  = 1;
}

int main() {
  int counter = 0;
  struct sigaction sigint_action = {};
  struct sigaction sigterm_action = {};
  struct sigaction sigusr1_action = {};
  struct sigaction sigusr2_action = {};
  
  sigterm_action.sa_handler = process_sigint_sigterm;
  sigint_action.sa_handler = process_sigint_sigterm;
  
  sigusr1_action.sa_handler = process_sigusr1;
  sigusr2_action.sa_handler = process_sigusr2;
  
  sigaction(SIGINT, &sigint_action, NULL);
  sigaction(SIGTERM, &sigterm_action, NULL);

  sigaction(SIGUSR1, &sigusr1_action, NULL);
  sigaction(SIGUSR2, &sigusr2_action, NULL);

  pid_t pid = getpid();

  printf("%d\n", pid);
  fflush(stdout);
  scanf("%d", &counter);

  while (1) {
    if(sigusr1) {
      ++counter;
      printf("%d\n", counter);
      fflush(stdout);
      sigusr1 = 0;
    }

    if(sigusr2) {
      counter *= -1;
      printf("%d\n", counter);
      fflush(stdout);
      sigusr2 = 0;
    }

    if (exit_command) {
      return 0;
    }
    pause();
  }
}
