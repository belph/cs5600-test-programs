#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <sys/file.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  assert(argc == 3);
  char pid_ns_path[64];
  char mnt_ns_path[64];
  char user_ns_path[64];
  char *ns_last_pid_path = "/proc/sys/kernel/ns_last_pid";
  int new_pid;
  snprintf(pid_ns_path, 64, "/proc/%s/ns/pid", argv[1]);
  snprintf(mnt_ns_path, 64, "/proc/%s/ns/mnt", argv[1]);
  snprintf(user_ns_path, 64, "/proc/%s/ns/user", argv[1]);
  sscanf(argv[2], "%d", &new_pid);
  int fd_ns = open(pid_ns_path, O_RDONLY, 0644);
  int fd_mnt_ns = open(mnt_ns_path, O_RDONLY, 0644);
  int fd_user_ns = open(user_ns_path, O_RDONLY, 0644);
  if (fd_ns < 0 || fd_mnt_ns < 0 || fd_user_ns < 0) {
    perror("open");
    return 1;
  }
  printf("PID: %d\n", (int)syscall(SYS_getpid));
  char orig_pid_ns[64];
  readlink("/proc/self/ns/pid", orig_pid_ns, 64);
  fprintf(stderr, "open 1: %d\n", fd_ns);
  if (setns(fd_user_ns, CLONE_NEWUSER) < 0) {
    perror("setns");
    return 1;
  }
  fprintf(stderr, "user\n");
  if (setns(fd_mnt_ns, CLONE_NEWNS) < 0) {
    perror("setns");
    return 1;
  }
  fprintf(stderr, "mnt\n");

  // NOTE: This does *not* change the namespace of *this*
  //       process, but it _does_ change it for new children
  if (setns(fd_ns, CLONE_NEWPID) < 0) {
    perror("setns");
    return 1;
  }
  fprintf(stderr, "pid\n");
  
  execl("/bin/zsh", "/bin/zsh", NULL);
  // int sysctl (int *name, int nlen, void *oldval,
  //            size_t *oldlenp, void *newval, size_t newlen);

  char cmd[64];

  // TODO: This all should take place in a child process, so that the
  //       correct file is locked by the kernel (this currently locks the
  //       old PID namespace's ns_last_pid file)
  int fd = open(ns_last_pid_path, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  

  if (flock(fd, LOCK_EX)) {
    close(fd);
    fprintf(stderr, "Can't lock file\n");
    return 1;
  }
  system("whoami");
  // We can't write to the /proc/sys file because we're still in
  // the original PID namespace!
  snprintf(cmd, 64, "sysctl kernel.ns_last_pid=%d", new_pid - 1);
  system(cmd);

  /*char buf[32];
  snprintf(buf, 32, "%d", new_pid - 1);
  if ((size_t)write(fd, buf, strlen(buf)) != strlen(buf)) {
    fprintf(stderr, "Can't write to buf\n");
    perror("write");
    return 1;
  }*/
  int forked_pid = fork();
  if (forked_pid == 0) {
    printf("child pid: %d\n", getpid());
    kill(1, SIGUSR1);
    exit(0);
  } else {
    printf("parent pid: %d\n\tchild: %d\n", getpid(), forked_pid);
  }

  /*if (flock(fd, LOCK_UN)) {
    fprintf(stderr, "can't unlock\n");
  }
  close(fd);*/
  close(fd_ns);
  close(fd_mnt_ns);
  close(fd_user_ns);
  return 0;
}
