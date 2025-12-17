#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>

#include "hr_sensor.h"

#define I2C_ADDRESS 0xAE
#define DEVICE_ID   0x15
#define ID	        0xFF

#define I2C_NODE DT_NODELABEL(hr_sense)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C_NODE);

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(hr_sensor, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(hr_sensor, LOG_LEVEL_INF);
#endif

int hr_sensor_init(void) {
    if (!i2c_is_ready_dt(&dev_i2c)) {
        LOG_ERR("HR sensor is not ready");
        return -1;
    }

    uint8_t id = 0;
    uint8_t regs[] = {ID};
    int ret = i2c_write_read_dt(&dev_i2c, regs, 1, &id, 1);

    if (ret != 0) {
        LOG_ERR("Could not read device ID (err %d)", ret);
        return -1;
    }

    if (id != DEVICE_ID) {
        LOG_ERR("Could not verify device ID");
        return -1;
    }

    LOG_INF("HR sensor successfully initialized");
    return 0;
}