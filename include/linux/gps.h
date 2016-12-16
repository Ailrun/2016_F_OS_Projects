#ifndef _LINUX_GPS_H
#define _LINUX_GPS_H

struct gps_location {
	double	latitude;
	double	longitude;
	float	accuracy; /* in meters */
};

#define __GPS_LOCATION_INITIALIZER(lat, lon, acc)	\
{							\
	.latitude	= lat,				\
	.longitude	= lon,				\
        .accuracy	= acc				\
}

#define DEFINE_GPS_LOCATION(name)			\
	struct gps_location name = __GPS_LOCATION_INITIALIZER(0, 0, 0)

#endif /* _LINUX_GPS_H */
