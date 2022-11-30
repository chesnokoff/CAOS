#include <signal.h>
#include <unistd.h>
#include <stdio.h>

volatile sig_atomic_t sigint = 0;
volatile sig_atomic_t sigterm = 0;

void process_sigint(int signum) {
  sigint = 1;
}

void process_sigterm(int signum) {
  sigterm = 1;
}

int main() {
  unsigned int counter = 0;

  struct sigaction sigint_action = {};
  struct sigaction sigterm_action = {};

  sigint_action.sa_handler = process_sigint;
  sigterm_action.sa_handler = process_sigterm;

  sigaction(SIGINT, &sigint_action, NULL);
  sigaction(SIGTERM, &sigterm_action, NULL);

  printf("%d\n", getpid());
  fflush(stdout);

  while (1) {
    if(sigint) {
      sigint = 0;
      counter++;
    }

    if (sigterm) {
      printf("%u\n", counter);
      return 0;
    }

    pause();
  }
}
