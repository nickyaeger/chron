#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "page.h"

/* GPIO device structures for buttons */
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

#define SW1_NODE DT_ALIAS(sw1)
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);

#define SW2_NODE DT_ALIAS(sw2)
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(SW2_NODE, gpios);

#define SW3_NODE DT_ALIAS(sw3)
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(SW3_NODE, gpios);

/* GPIO device structures for buttons */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

#define LED2_NODE DT_ALIAS(led2)
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

#define LED3_NODE DT_ALIAS(led3)
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(gpio, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(gpio, LOG_LEVEL_INF);
#endif

/* Button 0 callback function */
void button0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int ret;
	gpio_pin_toggle_dt(&led0);
	LOG_DBG("Button 0 pressed");
	if ((ret = k_work_submit_to_queue(&ui_workq, &button0_work)) < 0) {
		LOG_ERR("Failed to submit button 0 press to queue (err %d)", ret);
	}
}

/* Button 1 callback function */
void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int ret;
	gpio_pin_toggle_dt(&led1);
	LOG_DBG("Button 1 pressed");
	if ((ret = k_work_submit_to_queue(&ui_workq, &button1_work)) < 0) {
		LOG_ERR("Failed to submit button 0 press to queue (err %d)", ret);
	}
}

/* Button 2 callback function */
void button2_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int ret;
	gpio_pin_toggle_dt(&led2);
	LOG_DBG("Button 2 pressed");
	if ((ret = k_work_submit_to_queue(&ui_workq, &button2_work)) < 0) {
		LOG_ERR("Failed to submit button 0 press to queue (err %d)", ret);
	}
}

/* Button 3 callback function */
void button3_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int ret;
	gpio_pin_toggle_dt(&led3);
	LOG_DBG("Button 3 pressed");
	if ((ret = k_work_submit_to_queue(&ui_workq, &button3_work)) < 0) {
		LOG_ERR("Failed to submit button 0 press to queue (err %d)", ret);
	}
}

/* Define a variable of type static struct gpio_callback */
static struct gpio_callback button0_cb_data;
static struct gpio_callback button1_cb_data;
static struct gpio_callback button2_cb_data;
static struct gpio_callback button3_cb_data;

int gpio_init(void)
{
	int ret;

	/* Check button status */
	if (!device_is_ready(button0.port)) {
		LOG_ERR("Button 0 not ready");
		return -1;
	}
	if (!device_is_ready(button1.port)) {
		LOG_ERR("Button 1 not ready");
		return -1;
	}
	if (!device_is_ready(button2.port)) {
		LOG_ERR("Button 2 not ready");
		return -1;
	}
	if (!device_is_ready(button3.port)) {
		LOG_ERR("Button 3 not ready");
		return -1;
	}

	LOG_DBG("All buttons ready for use");

	/* Check LED status */
	if (!device_is_ready(led0.port)) {
		LOG_ERR("LED 0 not ready");
		return -1;
	}
	if (!device_is_ready(led1.port)) {
		LOG_ERR("LED 1 not ready");
		return -1;
	}
	if (!device_is_ready(led2.port)) {
		LOG_ERR("LED 2 not ready");
		return -1;
	}
	if (!device_is_ready(led3.port)) {
		LOG_ERR("LED 3 not ready");
		return -1;
	}

	LOG_DBG("All LEDs ready for use");

	/* Configure buttons */
	ret = gpio_pin_configure_dt(&button0, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure button 0 (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure button 1 (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_configure_dt(&button2, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure button 2 (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_configure_dt(&button3, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure button 3 (err %d)", ret);
		return -1;
	}

	LOG_DBG("All buttons successfully configured");

	/* Configure LEDs */
	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure led 0 (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure led 1 (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure led 2 (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure led 3 (err %d)", ret);
		return -1;
	}

	LOG_DBG("All LEDs successfully configured");

	/* Configure the interrupt on the buttons' pins */
	ret = gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure button 0 interrupt (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure button 1 interrupt (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_interrupt_configure_dt(&button2, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure button 2 interrupt (err %d)", ret);
		return -1;
	}
	ret = gpio_pin_interrupt_configure_dt(&button3, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure button 3 interrupt (err %d)", ret);
		return -1;
	}

	LOG_DBG("All button interrupts successfully configured");

	/* Initialize the static struct gpio_callback variables */
	gpio_init_callback(&button0_cb_data, button0_pressed, BIT(button0.pin));
	gpio_init_callback(&button1_cb_data, button1_pressed, BIT(button1.pin));
	gpio_init_callback(&button2_cb_data, button2_pressed, BIT(button2.pin));
	gpio_init_callback(&button3_cb_data, button3_pressed, BIT(button3.pin));

	/* Add the callback functions */
	gpio_add_callback(button0.port, &button0_cb_data);
	gpio_add_callback(button1.port, &button1_cb_data);
	gpio_add_callback(button2.port, &button2_cb_data);
	gpio_add_callback(button3.port, &button3_cb_data);

	LOG_INF("GPIO successfully initialized");
	return ret;
}
