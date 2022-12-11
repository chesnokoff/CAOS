#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char* code = NULL;
    size_t len = 0;
    ssize_t nread = getline(&code, &len, stdin);

    char* full_code = malloc((nread + 20) * sizeof(char));
    snprintf(full_code, nread + 20, "print(%s)", code);

    execlp("python", "python", "-c", full_code, NULL);
    perror("execlp");
    _exit(1);
}
