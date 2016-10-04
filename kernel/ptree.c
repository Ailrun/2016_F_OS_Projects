#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/prinfo.h>

asmlinkage long sys_ptree(void)
{
	struct prinfo p;
	printk("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state, p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);
	printk("Hello World\n");
	return 0;
}
