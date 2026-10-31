#include <zephyr/init.h>
#include <zephyr/kernel.h>

static int board_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(board_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);