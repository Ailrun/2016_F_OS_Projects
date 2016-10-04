#ifndef _LINUX_PRINFO_H
#define _LINUX_PRINFO_H
#include <linux/unistd.h>
#include <linux/types.h>

#define PRINFO_COMM_LENGTH 64

struct prinfo {
	long state;		/* current state of process */
	pid_t pid;		/* process id */
	pid_t parent_pid;	/* process id of parent */
	pid_t first_child_pid;	/* pid of oldest child */
	pid_t next_sibling_pid; /* pid of younger sibling */
	long uid;		/* user id of process owner */
	char comm[PRINFO_COMM_LENGTH];		/* name of program executed */
};

#endif /*_LINUX_PRINFO_H*/

