#include <linux/unistd.h>
#include <linux/kernel.h>

asmlinkage long sys_ptree(void)
{
	printk("Hello World\n");
	return 0;
}
