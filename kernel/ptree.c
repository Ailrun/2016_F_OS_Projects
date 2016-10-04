#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/prinfo.h>

static int walk_process_tree();
asmlinkage int sys_ptree(struct prinfo *buf, int *nr)
{
	int success_entries;
	int errorint;

	if (buf == NULL || nr == NULL) {
		errorint = -EINVAL;
		goto error;
	}
	
	struct prinfo p;
	printk("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state,
		 p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);
	printk("Hello World\n");

	success_entries = walk_process_tree();

	return success_entries;
error:
	return errorint;
}


/*
 TODO traverse the process tree data structures in pre-ordering order
 and copy process info into 'buf'
 return the total number of entries on success(?)
*/
static int walk_process_tree() {
	struct task_struct *task = get_current();

	read_lock(&tasklist_lock);

	for_each_process(task) {
	
	}

	read_unlock(&tasklist_lock);

	return 0;	
}
