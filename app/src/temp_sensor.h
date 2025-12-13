/**
 * @brief Initialize Temp Sensor
 *
 * @return 0 if successful, negative error code otherwise.
 */
int temp_sensor_init(void);

/**
 * @brief Read temperature from Temp Sensor
 *
 * @return Temperature value
 */
double temp_get(bool in_C);

/**
 * @brief Read presssure from Temp Sensor
 *
 * @return Pressure value
 */
double pressure_get(void);

/**
 * @brief Read humidity from Temp Sensor
 *
 * @return Humidity value
 */
double humidity_get(void);