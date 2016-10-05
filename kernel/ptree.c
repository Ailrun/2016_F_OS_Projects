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

static struct prinfo *ptree_buf = NULL;
static struct mutex ptree_lock;

/*
  traverse the process tree data structures in pre-order
  and copy process info into 'buf'
  return the total number of entries on success
*/
SYSCALL_DEFINE2(ptree, struct prinfo __user *, buf, int __user *, nr)
{
	int total_number = 0;
	int copy_number = 0;
	int non_copy_number = 0;
	int errorint = 0;
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


	walk_process_tree(&total_number);


	if (copy_from_user(&copy_number, nr, sizeof(int))) {
		errorint = -EFAULT;
		goto error;
	}
	copy_number = copy_number > total_number ? total_number : copy_number;

	non_copy_number = copy_to_user(buf, ptree_buf, copy_number*sizeof(struct prinfo))/sizeof(struct prinfo);

	copy_number -= non_copy_number;
	if (copy_to_user(nr, &copy_number, sizeof(int)) || non_copy_number) {
		errorint = -EFAULT;
		goto error;
	}

	mutex_unlock(&ptree_lock);


	printk(KERN_DEBUG "[OS_SNU_16] sys_ptree end\n");
	return total_number;
error:
	mutex_unlock(&ptree_lock);

	printk(KERN_DEBUG "[OS_SNU_16] sys_ptree error\n");
	return errorint;
}

static int walk_process_tree(int *total)
{
	struct task_struct *curr = NULL;
	struct task_struct *child = NULL;
	printk(KERN_DEBUG "[OS_SNU_16] walk_process_tree start\n");


	read_lock(&tasklist_lock);

	curr = &init_task;
	list_for_each_entry(child, &(curr->children), sibling)
		copy_in_preorder(child, total);

	read_unlock(&tasklist_lock);


	printk(KERN_DEBUG "[OS_SNU_16] walk_process_tree end\n");
	return 0;
}

static int copy_in_preorder(struct task_struct *curr, int *total)
{
	struct task_struct *child = NULL;
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
