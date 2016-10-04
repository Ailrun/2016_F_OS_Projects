#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/string.h>

#include <linux/prinfo.h>

#define PTREE_PAGE_SIZE 4096

static int walk_process_tree(struct prinfo *buf, int *nr);
static int copy_to_prinfo_from_task(struct prinfo *pr, struct task_struct *task);
static int copy_preorder(struct prinfo *buf, struct task_struct *cur, int n, int *result);

static struct prinfo *ptree_buff;
static struct mutex ptree_lock;

SYSCALL_DEFINE2(ptree, struct prinfo __user *, buf, int *, nr)
{
	int success_entries;
	int errorint;

	if (!ptree_buff) {
		ptree_buff = (prinfo *) kmalloc(PTREE_PAGE_SIZE, GFP_KERNEL);
		if (!ptree_buff) {
			errorint = -ENOMEM;
			goto error;
		}
		mutex_init(&ptree_mutex);
	}

	if (buf == NULL || nr == NULL || *nr < 1) {
		errorint = -EINVAL;
		goto error;
	}

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
	int result = 0;
	struct task_struct *curr = &init_task;

	read_lock(&tasklist_lock);

        copy_preorder(buf, curr, *nr, &result);
	*nr = result;

	read_unlock(&tasklist_lock);

	return 0;
}

static int copy_preorder(struct prinfo *buf, struct task_struct *curr, int n, int *result)
{
	int child_result;
	struct task_struct *child;

	*result = 0;

	if (n > 0) {
		copy_to_prinfo_from_task(buf, curr);
		(*result)++;

		list_for_each_entry(child, &(curr->children), sibling) {
		        copy_preorder(buf+*result, child, n-*result, &child_result);
			*result += child_result;

		}
	}

	return result;
}

static int copy_to_prinfo_from_task(struct prinfo *pr, struct task_struct *task)
{
	pr->state = task->state;
	pr->pid = task->pid;
	pr->uid = task->real_cred->uid;
	pr->parent_pid = task->real_parent->pid;

	if (!list_empty(&task->children))
		pr->first_child_pid = list_first_entry(&task->children, struct task_struct, sibling)->pid;
	else
		pr->first_child_pid = -1;

	if (!list_is_last(&task->sibling, &task->real_parent->children))
		pr->next_sibling_pid = list_first_entry(&task->sibling, struct task_struct, sibling)->pid;
	else
		pr->next_sibling_pid = -1;

	strncpy(pr->comm, task->comm, PRINFO_COMM_LENGTH);
	return 0;
}
