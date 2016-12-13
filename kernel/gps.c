#include <linux/gps.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc)
{
	return 0;
}

SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname,
		struct gps_location __user *, loc)
{
	return 0;
}
