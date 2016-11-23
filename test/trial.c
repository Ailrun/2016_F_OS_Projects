#include <stdio.h>
#include <linux/types.h>

#define SCHED_SETWEIGHT 384
#define SCHED_GETWEIGHT 385

int main(int argc, char* argv[])
{
	int i, number, weight;
	pid_t id = 0;
	clock_t start, end;

	if (argc != 3) {
		printf("Enter number and weight.");
		return -1;
	}

	number = atoi(argv[1]);
	weight = atoi(argv[2]);

	printf("set weight(success:0): %d\n", 
				syscall(SCHED_SETWEIGHT, id, weight));

	start = clock();
	if (number == 1) printf("1\n");
	while(number != 1){
		for(i = 2; i <= number; i++){
			if(number % i == 0){
				printf("%d ",i);
				number = number / i;
				break;
			}
		}
	}
	printf("\n");
	end = clock();
	printf("execution time: %.7f\n", (double)(end-start)/1000);

	return 0;
}
