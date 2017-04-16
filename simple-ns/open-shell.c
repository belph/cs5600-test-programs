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
#include <sys/wait.h>

// Copied from nsenter
static void continue_as_child(void) {
	pid_t child = fork();
	int status;
	pid_t ret;

	assert(child >= 0);

	/* Only the child returns */
	if (child == 0)
		return;

	for (;;) {
		ret = waitpid(child, &status, WUNTRACED);
		if ((ret == child) && (WIFSTOPPED(status))) {
			/* The child suspended so suspend us as well */
			kill(getpid(), SIGSTOP);
			kill(child, SIGCONT);
		} else {
			break;
		}
	}
	/* Return the child's exit code if possible */
	if (WIFEXITED(status)) {
		exit(WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		kill(getpid(), WTERMSIG(status));
	}
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  char pid_ns_path[64];
  char mnt_ns_path[64];
  char user_ns_path[64];
  char *ns_last_pid_path = "/proc/sys/kernel/ns_last_pid";
  snprintf(pid_ns_path, 64, "/proc/%s/ns/pid", argv[1]);
  snprintf(mnt_ns_path, 64, "/proc/%s/ns/mnt", argv[1]);
  snprintf(user_ns_path, 64, "/proc/%s/ns/user", argv[1]);
  int fd_ns = open(pid_ns_path, O_RDONLY, 0644);
  int fd_mnt_ns = open(mnt_ns_path, O_RDONLY, 0644);
  int fd_user_ns = open(user_ns_path, O_RDONLY, 0644);
  if (fd_ns < 0 || fd_mnt_ns < 0 || fd_user_ns < 0) {
    perror("open");
    return 1;
  }
  char orig_pid_ns[64];
  readlink("/proc/self/ns/pid", orig_pid_ns, 64);
  if (setns(fd_user_ns, CLONE_NEWUSER) < 0) {
    perror("setns");
    return 1;
  }
  if (setns(fd_mnt_ns, CLONE_NEWNS) < 0) {
    perror("setns");
    return 1;
  }

  // NOTE: This does *not* change the namespace of *this*
  //       process, but it _does_ change it for new children
  if (setns(fd_ns, CLONE_NEWPID) < 0) {
    perror("setns");
    return 1;
  }

  close(fd_ns);
  close(fd_mnt_ns);
  close(fd_user_ns);

  continue_as_child();
  char *shell = getenv("SHELL");
  execl(shell, shell, NULL);
  return 0;
}
