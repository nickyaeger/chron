/**
 * @brief Initialize IMU
 *
 * @return 0 if successful, negative error code otherwise.
 */
int imu_init(void);

/**
 * @brief Read from IMU pedometer
 *
 * @return Number of steps since last reset
 */
uint16_t steps_get(void);

/**
 * @brief Reset IMU pedometer
 */
void steps_reset(void);
