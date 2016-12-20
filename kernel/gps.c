#include <linux/gps.h>
#include <linux/syscalls.h>
#include <linux/errno.h>
#include <linux/namei.h>

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
	struct path path;
	struct inode *inode;
	struct gps_location nloc;
	int errchk;

	if (pathname == NULL || loc == NULL)
		return -EINVAL;

	if(user_path(pathname, &path))
		return -EINVAL;

	inode = path.dentry->d_inode;

	if(!inode->i_op->get_gps_location || inode->i_mode)
		return -ENODEV;

	errchk = inode->i_op->get_gps_location(inode, &nloc);
	path_put(&path);

	if(copy_to_user(loc, &nloc, sizeof(struct gps_location)))
		return -EFAULT;
	
	if(errchk)
		return errchk;
	else return -1;
}

void get_device_location(struct gps_location *loc)
{
	read_lock(&dev_loc_lock);

	memcpy(loc, &dev_loc, sizeof(dev_loc));

	read_unlock(&dev_loc_lock);
}
