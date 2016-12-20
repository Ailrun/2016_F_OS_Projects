#include <stdlib.h>
#include <stdio.h>
#include "file_loc.h"

int main(int argc, char **argv)
{
	char *path;
	struct gps_location *loc;
	int result;

	if(argc != 2) {
	        printf("Give me a path!");
	        exit(-1);
	}

	path = argv[1];
	loc = malloc(sizeof(struct gps_location));

	result = get_gps_location(path, loc);

	if(result) {
		printf("Error getting GPS\n");
		exit(-1);
	}

	printf("GPS coordinates: %lf, %lf\n", loc->latitude, loc->longitude);
	printf("Accuracy: %fm\n", loc->accuracy);
	printf("Google map link: https://www.google.com/maps/@%lf,%lf\n",
					loc->latitude, loc->longitude);

	free(loc);
	return 0;

}
