#include <linux/syscalls.h>
#include <linux/rotation.h>

SYSCALL_DEFINE1(set_rotation, struct dev_rotation __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_set_rotation start\n");

	return 0;
}

SYSCALL_DEFINE1(rotlock_read, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotlock_read start\n");

	return 0;
}

SYSCALL_DEFINE1(rotlock_write, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotlock_write start\n");

	return 0;
}

SYSCALL_DEFINE1(rotunlock_read, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotunlock_read start\n");

	return 0;
}

SYSCALL_DEFINE1(rotunlock_write, struct rotation_range __user *, rot)
{
	pr_debug("[OS_SNU_16] sys_rotunlock_write start\n");

	return 0;
}
