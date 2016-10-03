#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/prinfo.h>

int main(void)
{
	long res = syscall(384);
	printf("Res: %ld\nerror: %s\n", res, strerror(errno));

	return 0;
}
