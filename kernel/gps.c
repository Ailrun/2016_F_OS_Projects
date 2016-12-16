#include <linux/gps.h>
#include <linux/syscalls.h>
#include <linux/errno.h>

static DEFINE_RWLOCK(dev_loc_lock);
static DEFINE_GPS_LOCATION(dev_loc);

SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc)
{
        struct gps_location tmp_loc;

	if (loc == NULL)
		return -EINVAL;

	if (copy_from_user(&tmp_loc, loc, sizeof(tmp_loc)))
		return -EFAULT;

	write_lock(&dev_loc_lock);

	memcpy(&dev_loc, &tmp_loc, sizeof(tmp_loc));

	write_unlock(&dev_loc_lock);

	return 0;
}

SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname,
		struct gps_location __user *, loc)
{
	return 0;
}

void get_device_location(struct gps_location *loc)
{
	read_lock(&dev_loc_lock);

	memcpy(loc, &dev_loc, sizeof(dev_loc));

	read_unlock(&dev_loc_lock);
}
