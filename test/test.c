#define _GNU_SOURCE
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/prinfo.h>

int main(void)
{
        struct prinfo buff[20];
        int length = 20;
		long res = syscall(384, buff, &length);
        for (int i = 0; i < 20; i ++) {
                printf("Comm: %s\tPid: %d\n", buff[i].comm, buff[i].pid);
        }
		printf("Res: %ld\nerror: %s\n", res, strerror(errno));
		return 0;
}
