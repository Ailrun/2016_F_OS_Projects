#include <asm/current.h>

#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/mutex.h>

#include <linux/rotation.h>

#define INIT_ROTAREA(rotareap)						\
	do {								\
		rotareap = kmalloc(sizeof(struct __rotarea_t),		\
					GFP_KERNEL);			\
		rotareap->read_ref = 0;					\
		rotareap->write_ref = 0;				\
		rotareap->write_waiting = 0;				\
		init_waitqueue_head(&rotareap->wq);			\
	} while (0)
#define DEINIT_ROTAREA(rotareap)		\
	do {					\
		kfree(rotareap);		\
		rotareap = NULL;		\
	} while (0)

struct __rotarea_t {
	int read_ref;
	int write_ref;
	int write_waiting;
	wait_queue_head_t wq;
};

static DEFINE_MUTEX(rotarea_list_lock);
static struct __rotarea_t *(rotarea_list[361]) = {NULL, };

static DEFINE_MUTEX(dev_rotation_lock);
static struct dev_rotation sys_dev_rotation;

static int wake_up_with_count(wait_queue_head_t *q);
static int is_range_read_lockable(int low, int high);
static int is_range_write_lockable(int low, int high);

SYSCALL_DEFINE1(set_rotation, struct dev_rotation __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_set_rotation start\n");


	int degree;


	mutex_lock(&dev_rotation_lock);

	if (copy_from_user(&sys_dev_rotation, rot, sizeof(*rot)))
		return -1;

	degree = sys_dev_rotation.degree;

	mutex_unlock(&dev_rotation_lock);


	int wake_count = 0;

	mutex_lock(&rotarea_list_lock);

	if (rotarea_list[degree] != NULL)
	        wake_count = wake_up_with_count(&rotarea_list[degree]->wq);

	mutex_unlock(&rotarea_list_lock);


	pr_debug("[OS_SNU_16] sys_dev_rotation = %d\n", degree);

	pr_debug("[OS_SNU_16] sys_set_rotation end\n");
	return wake_count;
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
		pr_debug("[OS_SNU_16] start loop of sys_rotlock_read\n");
		mutex_lock(&rotarea_list_lock);

		for (d = low; d <= high; d++) {
			if (rotarea_list[d] == NULL)
				INIT_ROTAREA(rotarea_list[d]);
			prepare_to_wait(&rotarea_list[d]->wq,
					&w, TASK_INTERRUPTIBLE);
		}

		lockable = is_range_read_lockable(low, high);

		if (lockable) {
			for (d = low; d <= high; d++) {
				if (rotarea_list[d] == NULL)
					INIT_ROTAREA(rotarea_list[d]);
				rotarea_list[d]->read_ref++;
			}

			break;

		}

		mutex_unlock(&rotarea_list_lock);

		pr_debug("[OS_SNU_16] before schedule sys_rotlock_read\n");

		schedule();

		if (signal_pending(current))
		        goto signal;

		pr_debug("[OS_SNU_16] end loop of sys_rotlock_read\n");
	}

	for (d = low; d <= high; d++)
		finish_wait(&rotarea_list[d]->wq, &w);
	mutex_unlock(&rotarea_list_lock);


	pr_debug("[OS_SNU_16] sys_rotlock_read end\n");

	return 0;

signal:
	for (d = low; d <= high; d++)
		finish_wait(&rotarea_list[d]->wq, &w);
	return -1;
}

SYSCALL_DEFINE1(rotlock_write, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotlock_write start\n");


	struct rotation_range r;


	if (copy_from_user(&r, rot, sizeof(*rot)))
		return -1;

	int low = r.rot.degree - r.degree_range;
	int high = r.rot.degree + r.degree_range;

	if (low < 0 || high > 360)
		return -1;


	int lockable;
	DEFINE_WAIT(w);

	int first = 1;

	int d;

	for (;;) {
		pr_debug("[OS_SNU_16] start loop of sys_rotlock_write\n");
		mutex_lock(&rotarea_list_lock);

		for (d = low; d <= high; d++) {
			if (rotarea_list[d] == NULL)
				INIT_ROTAREA(rotarea_list[d]);

			if (first)
				rotarea_list[d]->write_waiting++;

			prepare_to_wait(&rotarea_list[d]->wq,
					&w, TASK_INTERRUPTIBLE);

		}

		first = 0;

		lockable = is_range_write_lockable(low, high);

		pr_debug("[OS_SNU_16] sys_rotlock_write lockable: %d\n",
				lockable);

		if (lockable) {
			for (d = low; d <= high; d++) {
				if (rotarea_list[d] == NULL)
					INIT_ROTAREA(rotarea_list[d]);
				rotarea_list[d]->write_ref++;
				rotarea_list[d]->write_waiting--;
			}

			break;

		}


		mutex_unlock(&rotarea_list_lock);

		pr_debug("[OS_SNU_16] before schedule sys_rotlock_write\n");

		schedule();

		if (signal_pending(current))
		        goto signal;

		pr_debug("[OS_SNU_16] end loop of sys_rotlock_write\n");
	}

	for (d = low; d <= high; d++)
		finish_wait(&rotarea_list[d]->wq, &w);
	mutex_unlock(&rotarea_list_lock);

	pr_debug("[OS_SNU_16] sys_rotlock_write end\n");
	return 0;

signal:
	for (d = low; d <= high; d++) {
		finish_wait(&rotarea_list[d]->wq, &w);
		rotarea_list[d]->write_waiting--;
	}
	return -1;
}

SYSCALL_DEFINE1(rotunlock_read, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotunlock_read start\n");


	struct rotation_range r;


	if (copy_from_user(&r, rot, sizeof(*rot)))
		return -1;

	int low = r.rot.degree - r.degree_range;
	int high = r.rot.degree + r.degree_range;

	if (low < 0 || high > 360)
		return -1;


	mutex_lock(&dev_rotation_lock);

	int degree = sys_dev_rotation.degree;

	mutex_unlock(&dev_rotation_lock);


	mutex_lock(&rotarea_list_lock);

	int d;

	for (d = low; d <= high; d++) {
		rotarea_list[d]->read_ref--;

		if (d == degree)
			wake_up_all(&rotarea_list[d]->wq);

		if (rotarea_list[d]->read_ref == 0 &&
		    rotarea_list[d]->write_ref == 0 &&
		    rotarea_list[d]->write_waiting == 0)
			DEINIT_ROTAREA(rotarea_list[d]);

	}

	mutex_unlock(&rotarea_list_lock);


	pr_debug("[OS_SNU_16] sys_rotunlock_read end\n");
	return 0;
}

SYSCALL_DEFINE1(rotunlock_write, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotunlock_write start\n");


	struct rotation_range r;


	if (copy_from_user(&r, rot, sizeof(*rot)))
		return -1;

	int low = r.rot.degree - r.degree_range;
	int high = r.rot.degree + r.degree_range;

	if (low < 0 || high > 360)
		return -1;


	mutex_lock(&dev_rotation_lock);

	int degree = sys_dev_rotation.degree;

	mutex_unlock(&dev_rotation_lock);


	mutex_lock(&rotarea_list_lock);

	int d;

	for (d = low; d <= high; d++) {
		rotarea_list[d]->write_ref--;

		if (d == degree)
			wake_up_all(&rotarea_list[d]->wq);

		if (!waitqueue_active(&rotarea_list[d]->wq) &&
		    rotarea_list[d]->write_ref == 0 &&
		    rotarea_list[d]->write_waiting == 0)
			DEINIT_ROTAREA(rotarea_list[d]);

	}

	mutex_unlock(&rotarea_list_lock);


	pr_debug("[OS_SNU_16] sys_rotunlock_write end\n");
	return 0;
}

int wake_up_with_count(wait_queue_head_t *q)
{
	int count = 0;

	unsigned long flags;

	spin_lock_irqsave(&q->lock, flags);

	wait_queue_t *curr, *next;

	list_for_each_entry_safe(curr, next, &q->task_list, task_list) {
		unsigned flags = curr->flags;

		count++;
		pr_debug("[OS_SNU_16] list loop wake_up_with_count: %d\n",
				count);

		if (curr->func(curr, TASK_NORMAL, 0, NULL) &&
		    (flags & WQ_FLAG_EXCLUSIVE))
			break;
	}

	spin_unlock_irqrestore(&q->lock, flags);

	return count;
}

int is_range_read_lockable(int low, int high)
{
	mutex_lock(&dev_rotation_lock);

	int degree = sys_dev_rotation.degree;

	mutex_unlock(&dev_rotation_lock);

	if (degree < low || degree > high)
		return 0;


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

int is_range_write_lockable(int low, int high)
{
	mutex_lock(&dev_rotation_lock);

	int degree = sys_dev_rotation.degree;

	mutex_unlock(&dev_rotation_lock);

	if (degree < low || degree > high)
		return 0;


	int not_lockable = 0;
	int d;

	for (d = low; d <= high; d++) {
		if (rotarea_list[d] != NULL) {
			not_lockable |= rotarea_list[d]->read_ref;
			not_lockable |= rotarea_list[d]->write_ref;
		}
	}

	return !not_lockable;
}
