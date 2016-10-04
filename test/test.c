#define _GNU_SOURCE
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/prinfo.h>

#define BUFF_LEN 200
#define nullnumber -1
int pindex = 0;
int pmax;
void print_ptree(struct prinfo *, int);
int main(void)
{
	struct prinfo *buff;
	buff = (struct prinfo*)malloc(BUFF_LEN*sizeof(struct prinfo));
	int length = BUFF_LEN;
	pmax = syscall(384, buff, &length);
	printf("length : %d\n",pmax);
	if(pmax == -EINVAL || pmax == -EFAULT) return 1; 
	
	print_ptree(buff, 0);

	return 0;
}

void print_ptree(struct prinfo *buff, int step)
{
	if(pindex >= pmax || pindex >= BUFF_LEN) return;
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
