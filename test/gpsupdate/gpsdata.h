#ifndef GPSDATA_H
#define GPSDATA_H

#define LOC_LEN 6

static struct gps_location loc_list[LOC_LEN] = {
	{
		.latitude=37.4590469,
		.longitude=126.9513903,
		.accuracy=17,
	}, //0
	{
		.latitude=37.4499332,
		.longitude=126.950296,
		.accuracy=25,
	},
	{
		.latitude=37.4765372,
		.longitude=126.979384,
		.accuracy=213,
	},
	{
		.latitude=37.4484592,
		.longitude=126.950328,
		.accuracy=20,
	},
	{
		.latitude=37.4620752,
		.longitude=126.955631,
		.accuracy=94,
	},
	{
		.latitude=37.4769772,
		.longitude=126.961553,
		.accuracy=155,
	}, //5
};

#endif /* GPSDATA_H */
