#ifndef GPSUPDATE_H
#define GPSUPDATE_H

#include <linux/gps.h>
#include <sys/syscall.h>

#define SYSCALL_SET_GPS_LOCATION 384

static inline long set_gps_location(struct gps_location *loc)
{
	return syscall(SYSCALL_SET_GPS_LOCATION, loc);
}

#endif /* GPSUPDATE_H */
