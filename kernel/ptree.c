#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/string.h>

#include <linux/prinfo.h>

static int walk_process_tree(struct prinfo *buf, int *nr);
static int copy_to_prinfo_from_task(struct prinfo *pr, struct task_struct *task);

asmlinkage int sys_ptree(struct prinfo *buf, int *nr)
{
	int success_entries;
	int errorint;

	if (buf == NULL || nr == NULL || *nr < 1) {
		errorint = -EINVAL;
		goto error;
	}

	/* struct prinfo p; */
	/* printk("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state, */
	/*	 p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid); */
	printk(KERN_EMERG "[OS_SNU_16] Hello World\n");

	success_entries = walk_process_tree(buf, nr);

	return success_entries;
error:
	return errorint;
}


/*
  TODO traverse the process tree data structures in pre-ordering order
  and copy process info into 'buf'
  return the total number of entries on success(?)
*/
static int walk_process_tree(struct prinfo *buf, int *nr)
{
	struct task_struct *task = NULL;
	int i = 0;

	read_lock(&tasklist_lock);

	for_each_process(task) {
		if (i >= *nr) {
			printk(KERN_EMERG "[OS_SNU_16] task list exceeds buffer\n");
			break;
		}
		if (task == NULL) {
			printk(KERN_EMERG "[OS_SNU_16] task is NULL\n");
			break;
		}
		printk(KERN_EMERG "[OS_SNU_16] %d\n", task->pid);
		copy_to_prinfo_from_task(buf+i, task);
		i++;
	}

	read_unlock(&tasklist_lock);

	return 0;
}

static int copy_to_prinfo_from_task(struct prinfo *pr, struct task_struct *task)
{
	pr->state = task->state;
	pr->pid = task->pid;
	pr->uid = task->real_cred->uid;
	pr->parent_pid = task->real_parent->pid;
	pr->first_child_pid = list_first_entry(&task->children, struct task_struct, sibling)->pid;
	pr->next_sibling_pid = list_first_entry(&task->sibling, struct task_struct, sibling)->pid;
	strncpy(pr->comm, task->comm, PRINFO_COMM_LENGTH);
	return 0;
}
