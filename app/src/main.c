#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include "gpio.h"

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);
#endif

/* Main routine */
int main(void) {
    LOG_INF("Starting main routine");
    gpio_init();
    return 0;
}