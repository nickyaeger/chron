#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#include "gpio.h"
#include "imu.h"
#include "page.h"
#include "temp_sensor.h"

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
    ui_init();
    imu_init();
    temp_sensor_init();
    temp_get(true);
    temp_get(false);
    pressure_get();
    return 0;
}
