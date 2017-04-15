#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>

int fork_with_ns() {
  int ret = syscall(SYS_clone, SIGCHLD | CLONE_NEWPID | CLONE_NEWUSER | CLONE_NEWNS, 0, NULL, NULL, 0);
  if (ret < 0) {
    perror("clone");
  }
  return ret;
}

void handle(int sig) {
  printf("got signal %d\n", sig);
}

int pid = 0;

void kill_handler(int sig) {
  if (pid != 0) {
    kill(pid, 9);
  }
  exit(1);
}

int getpid() {
  return syscall(SYS_getpid);
}

int main() {
  int pipe_fd[2];
  int val = 0;
  // 0x11
  printf("SIGCHLD: 0x%x\n", SIGCHLD);
  //char tmpdir[64];
  //snprintf(tmpdir, 64, "/tmp/child-root-XXXXXX");
  //char *td;
  if (pipe(pipe_fd) == -1) {
    perror("pipe");
    exit(1);
  }
  /*td = mkdtemp(tmpdir);
  if (td == NULL) {
    perror("mkdtemp");
    return 1;
  }*/
  int uid = getuid();
  int gid = getgid();
  printf("Real IDs: uid=%d; gid=%d\n", uid, gid);
  if ((pid = fork_with_ns())) {
    printf("pid of parent: %d\n", getpid());
    printf("pid of child: %d\n", pid);
    printf("parent val: %d\n", val);
    val--;
    printf("parent val: %d\n", val);
    signal(SIGINT, kill_handler);

    char filename[64];
    snprintf(filename, 64, "/proc/%d/uid_map", pid);
    char to_write_uid[32];
    char to_write_gid[32];
    snprintf(to_write_uid, 32, "0 %d 1\n", uid);
    snprintf(to_write_gid, 32, "0 %d 1\n", gid);
    int uid_fd = open(filename, O_RDWR);
    if (uid_fd < 0) {
      perror("open uid_map");
      exit(1);
    }
    if ((size_t)write(uid_fd, to_write_uid, strlen(to_write_uid)) != strlen(to_write_uid)) {
      perror("write uid_map");
      exit(1);
    }

    char cmd[128];
    //snprintf(cmd, 128, "echo '0 1000 1' > /proc/%d/uid_map", pid);
    //system(cmd);
    snprintf(cmd, 128, "echo /proc/%d/uid_map && cat /proc/%d/uid_map", pid, pid);
    system(cmd);
    close(uid_fd);

    snprintf(filename, 64, "/proc/%d/setgroups", pid);
    int sg_fd = open(filename, O_RDWR);
    if (sg_fd < 0) {
      perror("open setgroups");
      exit(1);
    }
    if ((size_t)write(sg_fd, "deny", 4) != 4) {
      perror("write setgroups");
      exit(1);
    }
    close(sg_fd);
    snprintf(filename, 64, "/proc/%d/gid_map", pid);
    int gid_fd = open(filename, O_RDWR);
    if (gid_fd < 0) {
      perror("open gid_map");
      exit(1);
    }
    
    if ((size_t)write(gid_fd, to_write_gid, strlen(to_write_gid)) != strlen(to_write_gid)) {
      perror("write gid_map");
      exit(1);
    }
    /*FILE *uid_file = fopen(filename, "rw");
    if (!uid_file) {
      perror("fopen");
      exit(1);
    }
    fprintf(uid_file, "0 1000 1\n");
    fclose(uid_file);*/
    
    snprintf(filename, 64, "/proc/%d/gid_map", pid);
    /*FILE *gid_file = fopen(filename, "rw");
    if (!gid_file) {
      perror("fopen");
      exit(1);
    }
    fprintf(gid_file, "0 1000 1\n");
    fclose(gid_file);*/
    //snprintf(cmd, 128, "echo '0 1000 1' > /proc/%d/gid_map", pid);
    //system(cmd);
    snprintf(cmd, 128, "echo /proc/%d/gid_map && cat /proc/%d/gid_map", pid, pid);
    close(gid_fd);
    system(cmd);
    close(pipe_fd[1]);

    uid = syscall(SYS_getuid);
    gid = syscall(SYS_getgid);
    printf("parent: uid: %d, gid: %d\n", uid, gid);
    scanf("%d", &val);
    kill(pid, 9);
  } else {
    close(pipe_fd[1]);
    char c;
    if (read(pipe_fd[0], &c, 1) != 0) {
      fprintf(stderr, "Failure in child: read from pipe returned != 0\n");
      exit(1);
    }
    /*if (unshare(CLONE_NEWUSER)) {
      perror("unshare user");
      exit(1);
    }
    if (unshare(CLONE_NEWPID)) {
      perror("unshare pid");
      exit(1);
    }
    if (unshare(CLONE_NEWNS)) {
      perror("unshare ns");
      exit(1);
    }*/
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    /*printf("New root: %s\n", tmpdir);
    if (chdir(tmpdir) < 0) {
      perror("chdir");
      exit(1);
    }*/

    /*if (mkdir("proc", 0555) < 0) {
      perror("mkdir");
      exit(1);
    }*/
    /*if (umount("/proc") < 0) {
      perror("umount /proc");
      exit(1);
    }*/
    if (mount("proc", "/proc", "proc", 0, NULL) < 0) {
      perror("mount");
      exit(1);
    }

    /*char filename[64];
    snprintf(filename, 64, "/proc/1/uid_map");
    FILE *uid_file = fopen(filename, "rw");
    if (!uid_file) {
      perror("fopen");
      exit(1);
    }
    fprintf(uid_file, "%d 0 1\n", (int)syscall(SYS_getuid));
    fclose(uid_file);
    snprintf(filename, 64, "/proc/1/gid_map");
    FILE *gid_file = fopen(filename, "rw");
    if (!gid_file) {
      perror("fopen");
      exit(1);
    }
    fprintf(gid_file, "%d 0 1\n", (int)syscall(SYS_getgid));
    fclose(gid_file);*/

    /*int uid_fd = open("/proc/self/uid_map", O_RDONLY);
    if (uid_file < 0) {
      perror("fopen");
      exit(1);
    }
    char buf[64];
    while ((read(uid_fd, buf, 64))) {
      fprintf(stderr, "%s", buf);
    }
    fprintf(stderr, "\n");
    close(uid_fd);*/
    system("echo /proc/1/uid_map && cat /proc/1/uid_map");
    system("echo /proc/1/gid_map && cat /proc/1/gid_map");
    uid = syscall(SYS_getuid);
    gid = syscall(SYS_getgid);
    printf("child: uid: %d, gid: %d\n", uid, gid);
    system("whoami");
    printf("pid within child: %d (uid: %d)\n", (int)syscall(SYS_getpid), (int)syscall(SYS_getuid));
    printf("child val: %d\n", val);
    val++;
    printf("child val: %d\n", val);
    signal(SIGUSR1, handle);
    if (!fork()) {
      printf("in forked child\n");
      if (execl("/bin/ps", "/bin/ps", "-a", NULL) < 0) {
        perror("exec");
        exit(1);
      }
      exit(0);
    } else {
      wait(NULL);
    }
    while(1) {
      sleep(20);
    }
  }
}
