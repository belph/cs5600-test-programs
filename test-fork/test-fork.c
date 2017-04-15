#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  if (fork()) {
    sleep(10);
    fprintf(stderr, "parent (%d)\n", getpid());
  } else {
    sleep(10);
    fprintf(stderr, "child (%d)\n", getpid());
  }
}
