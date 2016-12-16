#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <linux/gps.h>

#include "gpsupdate.h"
#include "gpsdata.h"

static void daemonize(void);
static int gps_rr(void);

int main(void)
{
	daemonize();

	while (1) {
		gps_rr();
	}

	return 0;
}

void daemonize(void)
{
	pid_t pid;

	pid = fork();
	if (pid < -1)
		exit(-1);
	if (pid > 0)
		exit(0);

	if (setsid() < 0)
		exit(-1);

	signal(SIGHUP,SIG_IGN);
	pid = fork();
	if (pid < -1)
		exit(-1);
	if (pid > 0)
		exit(0);

	umask(0);

	if (chdir("/") < 0)
		exit(-1);

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int gps_rr(void)
{
	static int loc_ind = LOC_LEN - 1;

	loc_ind = (loc_ind + 1) % LOC_LEN;

	return set_gps_location(loc_list[loc_ind]);
}
