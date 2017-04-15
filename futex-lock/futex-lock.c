#include <linux/futex.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>

// https://locklessinc.com/articles/futex_cheat_sheet/


static int gettid() {
  return syscall(SYS_gettid);
}

typedef struct lock_s {
  int lockval;
} lock_t;

void init_lock(lock_t *lock) {
  lock->lockval = 0;
}

int acquire_lock(lock_t *lock) {
  int lockval;
  if (!((lockval = __sync_val_compare_and_swap(&(lock->lockval), 0, gettid())))) {
    // value was 0; return
    return 0;
  }
  assert(lockval != gettid());
  while ((lockval = __sync_val_compare_and_swap(&(lock->lockval), 0, gettid()))) {
    syscall(SYS_futex, &(lock->lockval), FUTEX_WAIT, lockval, NULL);
  }
  return 0;
}

int release_lock(lock_t *lock) {
  int lockval = lock->lockval;
  if (lockval != gettid()) {
    fprintf(stderr, "WARNING: Attempted to unlock lock belonging to other thread (belongs to %d; this thread: %d)\n", lockval, gettid());
    return -1;
  }
  lock->lockval = 0;
  syscall(SYS_futex, &(lock->lockval), FUTEX_WAKE, INT_MAX, NULL);
  return 0;
}

// Sleep which is resilient to signals
void real_sleep(int n) {
  assert(n >= 0);
  while (n) {
    n = sleep(n);
  }
}

void *run_thread(void *lock_ptr) {
  lock_t *lock = lock_ptr;
  real_sleep(1);
  fprintf(stderr, "run_thread:%d: acquiring lock\n", gettid());
  acquire_lock(lock);
  fprintf(stderr, "run_thread:%d: acquired\n", gettid());
  if (release_lock(lock)) {
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "run_thread:%d: released\n", gettid());
  pthread_exit(0);
}

int main() {
  lock_t lock;
  init_lock(&lock);
  pthread_t thread;
  pthread_create(&thread, NULL, run_thread, &lock);
  fprintf(stderr, "main:%d:acquiring lock\n", gettid());
  acquire_lock(&lock);
  fprintf(stderr, "main:%d:acquired. sleeping\n", gettid());
  real_sleep(10);
  if (release_lock(&lock)) {
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "main:%d:released\n", gettid());
  void *x;
  pthread_join(thread, &x);
  exit(0);
}
