#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  pid_t pid;
  if ((pid = fork())) {
    sleep(10);
    fprintf(stderr, "parent (%d)\n", getpid());
    int status;
    int ret = waitpid(pid, &status, WUNTRACED);
    return (ret == pid) ? status : 1;
  } else {
    sleep(10);
    fprintf(stderr, "child (%d)\n", getpid());
  }
}
