#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char* expression = NULL;
  size_t len = 0;
  ssize_t nread = getline(&expression, &len, stdin);
  if (nread > 0 && '\n' == expression[nread - 1]) {
    expression[nread - 1] = 0;
  }
  char* format_string = 
"#include <stdio.h>\n\
#include <unistd.h>\n\
\n\
int main() {\n\
  int value = (%s);\n\
  if (0 > printf(\"%%d\\n\", value)) {\n\
    perror(\"Output error\");\n\
    _exit(1);\n\
  }\n\
}";
  char* full_code = malloc((nread + strlen(format_string)) * sizeof(char));
  if (NULL == full_code) {
    perror("Allocation error");
    free(full_code);
    _exit(1);
  }
  sprintf(full_code, format_string, expression);
  int file = open("calculation.c", O_CREAT | O_WRONLY | O_CLOEXEC, 0666);
  if (0 > file) {
    perror("Could not create file");
    free(full_code);
    _exit(1);
  }
  if (0 > write(file, full_code, strlen(full_code))) {
    perror("Could not write in file");
    free(full_code);
    close(file);
    _exit(1);
  }
  close(file);

  // Compilation
  pid_t pid = fork();
  if (-1==pid) { 
    perror("fork");
    free(full_code);
    exit(1);
  }
  if (0 == pid) {
    execlp("gcc", "gcc", "-o", "calculation", "calculation.c", NULL);
    perror("execlp");
    _exit(1);
  } else {
    int wstatus;
    waitpid(pid, &wstatus, 0);
    if (!(WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0)) {
      free(full_code);
      _exit(1);
    }
  }

  // Execution
  pid = fork();
  if (-1==pid) { 
    perror("fork");
    free(full_code);
    exit(1);
  }
  if (0 == pid) {
    execlp("./calculation", "./calculation", NULL);
    perror("execlp");
    _exit(1);
  } else {
    int wstatus;
    waitpid(pid, &wstatus, 0);
    if (!(WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0)) {
    free(full_code);
      _exit(1);
    }
  }
  remove("calculation");
  remove("calculation.c");
  free(full_code);
}
