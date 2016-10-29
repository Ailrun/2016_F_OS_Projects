#define SYSCALL_ROTLOCK_WRITE 386
#define SYSCALL_ROTUNLOCK_WRITE 388
#define SELECTOR_FILE_NAME "integer"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>

#include <linux/rotation.h>

void selector(int number);

int main(int argc, char **argv)
{
	int number = 0;

	if (argc != 2)
		goto usage;

	number = atoi(argv[1]);

	if (number < 1)
		goto usage;

	while (1) {
		selector(number);
		number++;
	}

	return 0;

usage:
	printf("Usage: ./selector <positive integer>\n");
	exit(1);
}

void selector(int number)
{
	struct rotation_range srrange = {{90}, 90};
	FILE *file;

	syscall(SYSCALL_ROTLOCK_WRITE, &srrange);
	file = fopen(SELECTOR_FILE_NAME, "w");

	if (file == NULL)
		goto open_error;

	fprintf(file, "%d\n", number);

	fclose(file);

	printf("%d\n", number);

	syscall(SYSCALL_ROTUNLOCK_WRITE, &srrange);

	return;
open_error:
	syscall(SYSCALL_ROTUNLOCK_WRITE, &srrange);
}
