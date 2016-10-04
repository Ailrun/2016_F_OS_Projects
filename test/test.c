#define _GNU_SOURCE
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/prinfo.h>

#define BUFF_LEN 20

int main(void)
{
	struct prinfo buff[BUFF_LEN];
	int length = BUFF_LEN;
	long res = syscall(384, buff, &length);

	if (res == 0) {
		printf("%20s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\n",
		       "comm", "pid", "state",
		       "Ppid",
		       "FCpid",
		       "NSpid",
		       "uid");

		for (int i = 0; i < BUFF_LEN; i ++) {
			printf("%20s\t%10d\t%10ld\t%10d\t%10d\t%10d\t%10d\n",
			       buff[i].comm, buff[i].pid, buff[i].state,
			       buff[i].parent_pid,
			       buff[i].first_child_pid,
			       buff[i].next_sibling_pid,
			       buff[i].uid);
		}
	}

	printf("Res: %ld\nerror: %s\n", res, strerror(errno));
	return 0;
}
