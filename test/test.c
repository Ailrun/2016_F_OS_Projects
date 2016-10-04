#define _GNU_SOURCE
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/prinfo.h>

#define BUFF_LEN 20
#define nullnumber -1
int pindex = 0;
void print_ptree(struct prinfo *, int);
int main(void)
{
	struct prinfo buff[BUFF_LEN];
	int length = BUFF_LEN;
	long res = syscall(384, buff, &length);

	/*if (res == 0) {
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
    */
	print_ptree(buff, 0);
	printf("Res: %ld\nerror: %s\n", res, strerror(errno));
	return 0;
}

void print_ptree(struct prinfo *buff, int step)
{
	if(pindex >= BUFF_LEN) return;
	int i;
	struct prinfo p = buff[pindex];
    for(i=0;i<step;++i) { printf("\t"); }
	printf("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state,
			p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);
	++pindex;
	if(p.first_child_pid != nullnumber)
		print_ptree(buff, step+1);
	if(p.next_sibling_pid != nullnumber)
		print_ptree(buff, step);
}
