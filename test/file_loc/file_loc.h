#ifndef FILE_LOC_H
#define FILE_LOC_H

#include <linux/gps.h>
#include <sys/syscall.h>

#define SYSCALL_GET_GPS_LOCATION 385

static inline int get_gps_location(const char *pathname,
				   struct gps_location *loc)
{
	return syscall(SYSCALL_GET_GPS_LOCATION, pathname, loc);
}

#endif
 
