#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>

#include "temp_sensor.h"

#define I2C_ADDRESS 0x77
#define DEVICE_ID 0x60
#define CTRLMEAS 0xF4
#define CALIB00	 0x88
#define CALIB06	 0x8E
#define CALIB25	 0xA1
#define CALIB26	 0xE1
#define ID	     0xD0
#define TEMPMSB	 0xFA
#define PRESSMSB 0xF7
#define HUMMSB	 0xFD
#define CONFIG_VALUE 0x93

#define I2C_NODE DT_NODELABEL(temp_sense)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C_NODE);

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(temp_sensor, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(temp_sensor, LOG_LEVEL_INF);
#endif

/* Sensor calibration values */
typedef struct {
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;

    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;

    uint8_t dig_h1;
    int16_t dig_h2;
    uint8_t dig_h3;
    int16_t dig_h4;
    int16_t dig_h5;
    int8_t dig_h6;
} sensor_data_t;

static sensor_data_t data;

/* Compensate current temperature using previously stored sensor calibration data */
static int32_t bme280_compensate_temp(int32_t adc_temp)
{
	int32_t var1, var2;

	var1 = (((adc_temp >> 3) - ((int32_t)data.dig_t1 << 1)) * ((int32_t)data.dig_t2)) >> 11;

	var2 = (((((adc_temp >> 4) - ((int32_t)data.dig_t1)) *
		   ((adc_temp >> 4) - ((int32_t)data.dig_t1))) >> 12) * 
           ((int32_t)data.dig_t3)) >> 14;

	return ((var1 + var2) * 5 + 128) >> 8;
}

/* Compensate current pressure using previously stored sensor calibration data */
static uint32_t bme280_compensate_press(int32_t adc_press) {
    uint8_t temp_val[3] = {0};
    int ret = i2c_burst_read_dt(&dev_i2c, TEMPMSB, temp_val, 3);

    if (ret != 0) {
        LOG_ERR("Could not read temperature");
        return -1;
    }

    int32_t adc_temp = (temp_val[0] << 12) | 
                       (temp_val[1] << 4) | 
                       ((temp_val[2] >> 4) & 0x0F);

    int32_t varA, varB;
    varA = ((((adc_temp >> 3) - ((int32_t)data.dig_t1 << 1))) * ((int32_t)data.dig_t2)) >> 11;
    varB = (((((adc_temp >> 4) - ((int32_t)data.dig_t1)) * ((adc_temp >> 4) - ((int32_t)data.dig_t1)))
    >> 12) * ((int32_t)data.dig_t3)) >> 14;
    int32_t t_fine = varA + varB;
                       
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)data.dig_p6;
    var2 = var2 + ((var1 * (int64_t)data.dig_p5) << 17);
    var2 = var2 + (((int64_t)data.dig_p4) << 35);
    var1 = ((var1 * var1 * (int64_t)data.dig_p3) >> 8) + ((var1 * (int64_t)data.dig_p2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)data.dig_p1) >> 33;
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_press;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)data.dig_p9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)data.dig_p8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)data.dig_p7) << 4);
    return (uint32_t)p;
}

/* Compensate current pressure using previously stored sensor calibration data */
static uint32_t bme280_compensate_hum(int32_t adc_hum) {
    uint8_t temp_val[3] = {0};
    int ret = i2c_burst_read_dt(&dev_i2c, TEMPMSB, temp_val, 3);

    if (ret != 0) {
        LOG_ERR("Could not read temperature");
        return -1;
    }

    int32_t adc_temp = (temp_val[0] << 12) | 
                       (temp_val[1] << 4) | 
                       ((temp_val[2] >> 4) & 0x0F);

    int32_t varA, varB;
    varA = ((((adc_temp >> 3) - ((int32_t)data.dig_t1 << 1))) * ((int32_t)data.dig_t2)) >> 11;
    varB = (((((adc_temp >> 4) - ((int32_t)data.dig_t1)) * ((adc_temp >> 4) - ((int32_t)data.dig_t1)))
    >> 12) * ((int32_t)data.dig_t3)) >> 14;
    int32_t t_fine = varA + varB;
    
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_hum << 14) - (((int32_t)data.dig_h4) << 20) - (((int32_t)data.dig_h5) *
    v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *
    ((int32_t)data.dig_h6)) >> 10) * (((v_x1_u32r * ((int32_t)data.dig_h3)) >> 11) +
    ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)data.dig_h2) +
    8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
    ((int32_t)data.dig_h1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)(v_x1_u32r >> 12);
}

int temp_sensor_init(void) {
    if (!i2c_is_ready_dt(&dev_i2c)) {
        LOG_ERR("Temp sensor is not ready");
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

    uint8_t t_values[6];
    uint8_t p_values[18];
    uint8_t h_values[8];

    /* Get temperature calibration values */
    ret = i2c_burst_read_dt(&dev_i2c, CALIB00, t_values, 6);

    if (ret != 0) {
        LOG_ERR("Could not read calibration registers");
        return -1;
    }

    data.dig_t1 = ((uint16_t)t_values[1]) << 8 | t_values[0];
	data.dig_t2 = ((uint16_t)t_values[3]) << 8 | t_values[2];
	data.dig_t3 = ((uint16_t)t_values[5]) << 8 | t_values[4];

    /* Get pressure calibration values */
    ret = i2c_burst_read_dt(&dev_i2c, CALIB06, p_values, 18);

    if (ret != 0) {
        LOG_ERR("Could not read calibration registers");
        return -1;
    }

    data.dig_p1 = ((uint16_t)p_values[1]) << 8 | p_values[0];
    data.dig_p2 = ((uint16_t)p_values[3]) << 8 | p_values[2];
    data.dig_p3 = ((uint16_t)p_values[5]) << 8 | p_values[4];
    data.dig_p4 = ((uint16_t)p_values[7]) << 8 | p_values[6];
    data.dig_p5 = ((uint16_t)p_values[9]) << 8 | p_values[8];
    data.dig_p6 = ((uint16_t)p_values[11]) << 8 | p_values[10];
    data.dig_p7 = ((uint16_t)p_values[13]) << 8 | p_values[12];
    data.dig_p8 = ((uint16_t)p_values[15]) << 8 | p_values[14];
    data.dig_p9 = ((uint16_t)p_values[17]) << 8 | p_values[16];

    /* Get humidity calibration values */
    ret = i2c_burst_read_dt(&dev_i2c, CALIB25, h_values, 1);

    if (ret != 0) {
        LOG_ERR("Could not read calibration registers");
        return -1;
    }

    ret = i2c_burst_read_dt(&dev_i2c, CALIB26, h_values+1, 7);

    if (ret != 0) {
        LOG_ERR("Could not read calibration registers");
        return -1;
    }

    for (int i = 0; i < 8; i++) {
        LOG_INF("h_values[%d] = %x", i, h_values[i]);
    }

    data.dig_h1 = (uint16_t)h_values[0];
    data.dig_h2 = ((uint16_t)h_values[2]) << 8 | h_values[1];
    data.dig_h3 = (uint16_t)h_values[3];
    data.dig_h4 = ((uint16_t)h_values[4]) << 4 | (h_values[5] & 0x0F);
    data.dig_h5 = ((uint16_t)h_values[6]) << 4 | ((h_values[5] >> 4) & 0x0F);
    data.dig_h6 = (uint8_t)h_values[7];

    LOG_INF("data.dig_h1 = %x", data.dig_h1);
    LOG_INF("data.dig_h2 = %x", data.dig_h2);
    LOG_INF("data.dig_h3 = %x", data.dig_h3);
    LOG_INF("data.dig_h4 = %x", data.dig_h4);
    LOG_INF("data.dig_h5 = %x", data.dig_h5);
    LOG_INF("data.dig_h6 = %x", data.dig_h6);

    /* Configure sensor */
    uint8_t sensor_config[] = {CTRLMEAS, CONFIG_VALUE};
    ret = i2c_write_dt(&dev_i2c, sensor_config, 2);

    if (ret != 0) {
        LOG_ERR("Could not write to config registers");
        return -1;
    }

    LOG_INF("Temp sensor successfully initialized");
    return 0;
}

double temp_get(bool in_C) {
    uint8_t temp_val[3] = {0};
    int ret = i2c_burst_read_dt(&dev_i2c, TEMPMSB, temp_val, 3);

    if (ret != 0) {
        LOG_ERR("Could not read temperature");
        return -1;
    }

    int32_t adc_temp = (temp_val[0] << 12) | 
                       (temp_val[1] << 4) | 
                       ((temp_val[2] >> 4) & 0x0F);

    int32_t comp_temp = bme280_compensate_temp(adc_temp);

    double cTemp = (double)((float)comp_temp / 100.0f);
	double fTemp = cTemp * 1.8 + 32;

    if (in_C) {
        return cTemp;
    } else {
        return fTemp;
    }
}

double pressure_get(void) {
    uint8_t press_val[3] = {0};
    int ret = i2c_burst_read_dt(&dev_i2c, PRESSMSB, press_val, 3);

    if (ret != 0) {
        LOG_ERR("Could not read pressure");
        return -1;
    }

    int32_t adc_press = (press_val[0] << 12) | 
                        (press_val[1] << 4) | 
                        ((press_val[2] >> 4) & 0x0F);

    int32_t comp_press = bme280_compensate_press(adc_press);
    double pressure = (double)(comp_press >> 8) / (double)1000;
    return pressure;
}

double humidity_get(void) {
    uint8_t hum_val[2] = {0};
    int ret = i2c_burst_read_dt(&dev_i2c, HUMMSB, hum_val, 2);

    if (ret != 0) {
        LOG_ERR("Could not read humidity");
        return -1;
    }

    int32_t adc_hum = (hum_val[0] << 8) | hum_val[1];

    int32_t comp_hum = bme280_compensate_hum(adc_hum);
    double humidity = (double)comp_hum / (double)1024;
    return humidity;
}