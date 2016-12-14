#ifndef _LINUX_GPS_H
#define _LINUX_GPS_H

struct gps_location {
	double	latitude;
	double	longitude;
	float	accuracy; /* in meters */
};

#endif /* _LINUX_GPS_H */
