#define SYSCALL_ROTLOCK_READ 385
#define SYSCALL_ROTUNLOCK_READ 387
#define TRIAL_FILE_NAME "integer"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>

#include <linux/rotation.h>

static void trial(void);

int main(void)
{
	while (1)
		trial();

	return 0;
}

void trial(void)
{
	struct rotation_range trrange = {{30}, 30};
	int file;
	char buf[128];
	int number;
	int exp, prime;
	int first = 1;

	syscall(SYSCALL_ROTLOCK_READ, &trrange);
	file = open(TRIAL_FILE_NAME, O_RDONLY);

	if (file < 0)
		goto open_error;

	read(file, buf, 128);

	number = atoi(buf);
	exp = 0;
	printf("trial: ");

	for (prime = 2; number != 1;) {
		if (number % prime == 0) {
			exp++;
			number /= prime;

		} else if (exp != 0) {
			if (!first)
				printf("*");
			printf("%d^%d", prime, exp);

			first = 0;
			prime++;
			exp = 0;

		} else
			prime++;

	}

	if (!first)
		printf("*");
	printf("%d^%d\n", prime, exp);

	close(file);
	syscall(SYSCALL_ROTUNLOCK_READ, &trrange);

	return;
open_error:
	syscall(SYSCALL_ROTUNLOCK_READ, &trrange);
}
