#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>

#include "imu.h"

#define I2C_ADDRESS 0x6A
#define DEVICE_ID 0x6C
#define WHO_AM_I 0x0F
#define EMB_FUNC_ENABLE 0x01
#define ACCEL_ENABLE 0x10
#define PEDOMETER_ENABLE 0x04
#define PEDOMETER_INIT 0x66
#define STEPS_COUNT 0x62
#define STEPS_RESET 0x64

#define I2C_NODE DT_NODELABEL(imu)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C_NODE);

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(imu, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(imu, LOG_LEVEL_INF);
#endif

int imu_init(void) {
    if (!i2c_is_ready_dt(&dev_i2c)) {
        LOG_ERR("IMU is not ready");
        return -1;
    }

    /* Disable embedded function registers to access WHO_AM_I, Accel */
    int ret;
    uint8_t emb_func_config_disable[] = {EMB_FUNC_ENABLE, 0x0};
    ret = i2c_write_dt(&dev_i2c, emb_func_config_disable, 2);
    if (ret < 0) {
        LOG_ERR("Could not disable embedded function registers (err %d)", ret);
        return -1;
    }

    /* Verify ID */
    uint8_t reg = WHO_AM_I;
    uint8_t id = 0;
    ret = i2c_write_read_dt(&dev_i2c, &reg, sizeof(reg), &id, sizeof(id));
    if (ret < 0) {
        LOG_ERR("Could not read device ID (err %d)", ret);
        return -1;
    }

    if (id != DEVICE_ID) {
        LOG_ERR("Could not verify device ID");
        return -1;
    }

    /* Enable accelerometer */
    uint8_t accel_config[] = {ACCEL_ENABLE, 0x10};
    ret = i2c_write_dt(&dev_i2c, accel_config, 2);
    if (ret < 0) {
        LOG_ERR("Could not enable accelerometer (err %d)", ret);
        return -1;
    }

    /* Enable embedded function registers */
    uint8_t emb_func_config_enable[] = {EMB_FUNC_ENABLE, 0x80};
    ret = i2c_write_dt(&dev_i2c, emb_func_config_enable, 2);
    if (ret < 0) {
        LOG_ERR("Could not enable embedded function registers (err %d)", ret);
        return -1;
    }

    /* Enable pedometer */
    uint8_t pedom_enable_config[] = {PEDOMETER_ENABLE, 0x08};
    ret = i2c_write_dt(&dev_i2c, pedom_enable_config, 2);
    if (ret < 0) {
        LOG_ERR("Could not enable pedometer (err %d)", ret);
        return -1;
    }

    /* Initialize pedometer algorithm */
    uint8_t pedom_init_config[] = {PEDOMETER_INIT, 0x08};
    ret = i2c_write_dt(&dev_i2c, pedom_init_config, 2);
    if (ret < 0) {
        LOG_ERR("Could not initialize pedometer (err %d)", ret);
        return -1;
    }
    
    LOG_INF("IMU successfully initialized");
    return 0;
}

uint16_t steps_get(void) {
    uint8_t reg = STEPS_COUNT;
    uint8_t steps[] = {0, 0};
    int ret = i2c_burst_read_dt(&dev_i2c, reg, steps, 2);
    if (ret < 0) {
        LOG_ERR("Failed to read from pedometer (err %d)", ret);
    }
    uint16_t res = ((uint16_t)steps[1] << 8) | steps[0];
    
    LOG_DBG("Step counter: %d", res);
    return res;
}

void steps_reset(void) {
    int ret;
    uint8_t config[] = {STEPS_RESET, 0};
    ret = i2c_write_read_dt(&dev_i2c, config, sizeof(config[0]), config + 1, sizeof(config[1]));
    if (ret < 0) {
        LOG_ERR("Failed to read from IMU (err %d)", ret);
    }
    config[1] |= 0x80;
    ret = i2c_write_dt(&dev_i2c, config, 2);
    if (ret < 0) {
        LOG_ERR("Failed to write to IMU (err %d)", ret);
    }
    LOG_DBG("Step counter reset");
}
