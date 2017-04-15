#include <stdio.h>
#include <sys/types.h>
#include <syscall.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mtx;

int gettid() {
  return syscall(SYS_gettid);
}

void *unlock(void *dontcare) {
  sleep(30);
  int err = pthread_mutex_unlock(&mtx);
  printf("err is: %d; %d; %d\n", err, getpid(), gettid());
  if (err) {
    perror("pthread");
  }
  return NULL;
}

int main() {
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  pthread_mutex_init(&mtx, &attr);
  pthread_t thread;
  pthread_create(&thread, NULL, unlock, NULL);
  fprintf(stderr, "pre\n");
  pthread_mutex_lock(&mtx);
  unlock(NULL);
}
