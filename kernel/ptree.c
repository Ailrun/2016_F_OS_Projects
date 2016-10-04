#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/prinfo.h>

#include <asm/uaccess.h>
#include <asm/unistd.h>

#define PTREE_PAGE_SIZE 4096

static int walk_process_tree(int *total);
static int copy_in_preorder(struct task_struct *cur, int *total);
static int copy_to_prinfo_from_task(struct prinfo *pr, 
					struct task_struct *task);

static struct prinfo *ptree_buf;
static struct mutex ptree_lock;

/*
  traverse the process tree data structures in pre-order
  and copy process info into 'buf'
  return the total number of entries on success
*/
SYSCALL_DEFINE2(ptree, struct prinfo __user *, buf, int __user *, nr)
{
	int total_entries = 0, copy_entries = 0;
	int errorint;

	printk(KERN_DEBUG "[OS_SNU_16] sys_ptree start\n");

	if (!ptree_buf) {
		ptree_buf = (struct prinfo *) kmalloc(PTREE_PAGE_SIZE, GFP_KERNEL);
		if (!ptree_buf)
		        return -ENOMEM;
		mutex_init(&ptree_lock);
	}

	if (buf == NULL || nr == NULL || *nr < 1)
		return -EINVAL;

	mutex_lock(&ptree_lock);

	walk_process_tree(&total_entries);

	if (copy_entries = copy_to_user(buf, ptree_buf, *nr*sizeof(struct prinfo))/sizeof(struct prinfo)) {
		errorint = -EFAULT;
		goto error;
	};

	if (copy_to_user(nr, &copy_entries, sizeof(int))) {
		errorint = -EFAULT;
		goto error;
	}

	mutex_unlock(&ptree_lock);

	printk(KERN_DEBUG "[OS_SNU_16] sys_ptree end\n");
	return total_entries;
error:
	mutex_unlock(&ptree_lock);

	printk(KERN_DEBUG "[OS_SNU_16] sys_ptree error\n");
	return errorint;
}

static int walk_process_tree(int *total)
{
	struct task_struct *curr = NULL;

	printk(KERN_DEBUG "[OS_SNU_16] walk_process_tree start\n");

	read_lock(&tasklist_lock);

	curr = &init_task;
        copy_in_preorder(curr, total);

	read_unlock(&tasklist_lock);

	printk(KERN_DEBUG "[OS_SNU_16] walk_process_tree end\n");
	return 0;
}

static int copy_in_preorder(struct task_struct *curr, int *total)
{
	struct task_struct *child;

	printk(KERN_DEBUG "[OS_SNU_16] copy_in_preorder start\n");

	if (*total < PTREE_PAGE_SIZE/sizeof(struct prinfo)) {
		copy_to_prinfo_from_task(ptree_buf + *total, curr);
		(*total)++;

		printk(KERN_DEBUG "[OS_SNU_16] total in copy_in_preorder: %d\n", *total);

		list_for_each_entry(child, &(curr->children), sibling)
		        copy_in_preorder(child, total);
	}

	printk(KERN_DEBUG "[OS_SNU_16] copy_in_preorder end\n");
	return 0;
}

static int copy_to_prinfo_from_task(struct prinfo *pr, 
					struct task_struct *task)
{
	printk(KERN_DEBUG "[OS_SNU_16] copy_to_prinfo_from_task start\n");

	pr->state = task->state;
	pr->pid = task->pid;
	pr->uid = task->real_cred->uid;
	pr->parent_pid = task->real_parent->pid;

	if (!list_empty(&task->children))
		pr->first_child_pid = list_first_entry(&task->children, 
					struct task_struct, sibling)->pid;
	else
		pr->first_child_pid = -1;

	if (!list_is_last(&task->sibling, &task->real_parent->children))
		pr->next_sibling_pid = list_first_entry(&task->sibling, 
					struct task_struct, sibling)->pid;
	else
		pr->next_sibling_pid = -1;

	strncpy(pr->comm, task->comm, PRINFO_COMM_LENGTH);

	printk(KERN_DEBUG "[OS_SNU_16] copy_to_prinfo_from_task end\n");
	return 0;
}
