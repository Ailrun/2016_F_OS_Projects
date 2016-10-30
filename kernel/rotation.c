#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/mutex.h>

#include <linux/rotation.h>

#define INIT_ROTAREA(rotareap)						\
	do {								\
		rotareap = kmalloc(sizeof(rotarea_t), GFP_KERNEL);	\
		rotareap->read_ref = 0;					\
		rotareap->write_ref = 0;				\
		rotareap->write_waiting = 0;				\
		init_waitqueue_head(&rotareap->wq);			\
	} while(0)
#define DEINIT_ROTAREA(rotareap)		\
	do {					\
		kfree(rotareap);		\
	} while (0)

struct __rotarea_t {
	int read_ref;
	int write_ref;
	int write_waiting;
	wait_queue_head_t wq;
};

typedef struct __rotarea_t rotarea_t;

static DEFINE_MUTEX(rotarea_list_lock);
static rotarea_t *(rotarea_list[361]);

static DEFINE_MUTEX(dev_rotation_lock);
static struct dev_rotation sys_dev_rotation;


static int is_range_read_lockable(int low, int high);

SYSCALL_DEFINE1(set_rotation, struct dev_rotation __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_set_rotation start\n");


	int degree;

	
	mutex_lock(&dev_rotation_lock);

	if (copy_from_user(&sys_dev_rotation, rot, sizeof(*rot)))
		return -1;

	degree = sys_dev_rotation.degree;

	mutex_unlock(&dev_rotation_lock);


	mutex_lock(&rotarea_list_lock);

	if (rotarea_list[degree] != NULL &&
	    rotarea_list[degree]->read_ref == 0 &&
	    rotarea_list[degree]->write_waiting == 0)
		wake_up(&rotarea_list[degree]->wq);

	mutex_unlock(&rotarea_list_lock);


	pr_debug("[OS_SNU_16] sys_dev_rotation = %d\n", degree);

	pr_debug("[OS_SNU_16] sys_set_rotation end\n");
	return 0;
}

SYSCALL_DEFINE1(rotlock_read, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotlock_read start\n");


	struct rotation_range r;


	if (copy_from_user(&r, rot, sizeof(*rot)))
		return -1;


	int low = r.rot.degree - r.degree_range;
	int high = r.rot.degree + r.degree_range;

	if (low < 0 || high > 360)
		return -1;



	int lockable;
	DEFINE_WAIT(w);

	int d;

	for (;;) {
		mutex_lock(&rotarea_list_lock);

		for (d = low; d <= high; d++) {
			if (rotarea_list[d] == NULL)
				INIT_ROTAREA(rotarea_list[d]);
			prepare_to_wait(&rotarea_list[d]->wq,
					&w, TASK_INTERRUPTIBLE);
		}

		lockable = is_range_read_lockable(low, high);

		if (lockable < 0) {
			for (d = low; d <= high; d++) {
				if (rotarea_list[d] == NULL)
					INIT_ROTAREA(rotarea_list[d]);
				rotarea_list[d]->read_ref++;
			}

			break;
			
		}

		mutex_unlock(&rotarea_list_lock);
		
		schedule();
	}

	for (d = low; d <= high; d++)
		finish_wait(&rotarea_list[d]->wq, &w);
	mutex_unlock(&rotarea_list_lock);


	pr_debug("[OS_SNU_16] sys_rotlock_read end\n");

	return 0;
}

SYSCALL_DEFINE1(rotlock_write, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotlock_write start\n");

	pr_debug("[OS_SNU_16] sys_rotlock_write end\n");
	return 0;
}

SYSCALL_DEFINE1(rotunlock_read, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotunlock_read start\n");

	pr_debug("[OS_SNU_16] sys_rotunlock_read end\n");
	return 0;
}

SYSCALL_DEFINE1(rotunlock_write, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotunlock_write start\n");

	pr_debug("[OS_SNU_16] sys_rotunlock_write end\n");
	return 0;
}

int is_range_read_lockable(int low, int high)
{
	int not_lockable = 0;
	int d;

	for (d = low; d <= high; d++) {
		if (rotarea_list[d] != NULL) {
		        not_lockable |= rotarea_list[d]->write_ref;
		        not_lockable |= rotarea_list[d]->write_waiting;
		}
	}
	
	return !not_lockable;
}
