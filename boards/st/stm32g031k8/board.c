#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

static int board_stm32g031k8_init_hook(void){
    return printf("Board Initialized");
}