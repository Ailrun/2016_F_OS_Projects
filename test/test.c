#define _GNU_SOURCE
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/prinfo.h>

#define BUFF_LEN 800
#define nullnumber 0

int pindex;

static void print_ptree(struct prinfo *buff, int step, int *length);

int main(int argc, char **argv)
{
	struct prinfo *buff;
	int length;
	int err;

	if (argc == 2) {
		length = atoi(argv[1]);
		buff = malloc(length * sizeof(struct prinfo));

		err = syscall(384, buff, &length);
		if (err == -EINVAL || err == -EFAULT)
			return 1;

		print_ptree(buff, 0, &length);

		return 0;
	} else {
		printf("Usage: ./test <buffer length>\n");
		exit(1);
	}
}

void print_ptree(struct prinfo *buff, int step, int *length)
{
	int i;
	struct prinfo p;

	if (pindex >= *length)
		return;

	p = buff[pindex];
	for (i = 0; i < step; ++i)
		printf("\t");
	printf("%s,%d,%ld,%d,%d,%d,%d\n",
			p.comm, p.pid, p.state,
			p.parent_pid,
			p.first_child_pid, p.next_sibling_pid, p.uid);

	++pindex;

	if (p.first_child_pid != nullnumber)
		print_ptree(buff, step+1, length);
	if (p.next_sibling_pid != nullnumber)
		print_ptree(buff, step, length);
}
