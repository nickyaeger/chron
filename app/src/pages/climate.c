#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "../temp_sensor.h"
#include "../page.h"
#include "climate.h"
#include "summary.h"

#define STACKSIZE 1024
#define READ_TPH_PRIORITY 7

/* Climate page functions */
static void on_enter(void);
static void on_exit(void);
static void climate_input(ui_button_t button);

/* Settings page threads */
static void read_tph(void);
K_THREAD_DEFINE(read_tph_id, STACKSIZE, read_tph, NULL, NULL, NULL, READ_TPH_PRIORITY, 0, SYS_FOREVER_MS);

/* Activity page definition */
static char name[] = "Activity";
ui_page_t climate = { .on_enter = on_enter, .on_exit = on_exit, .handle_input = climate_input, .info = (void*)name };

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(climate, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(climate, LOG_LEVEL_INF);
#endif

static void on_enter(void) {
    LOG_DBG("Current page: CLIMATE");
    k_thread_start(read_tph_id);
    k_thread_resume(read_tph_id);
}

static void on_exit(void) {
    k_thread_suspend(read_tph_id);
}

/* Climate page input handler */
static void climate_input(ui_button_t button) {
    switch (button) {
        case BTN_PREV:
            break;
        case BTN_NEXT:
            break;
        case BTN_BACK:
            set_carousel_view(CLIMATE_VIEW);
            ui_pop();
            break;
        case BTN_SELECT:
            break;
        default:
            break;
    }
}

/* Read temp/pressure/humidity thread */
static void read_tph(void) {
    while (1) {
        double temp = temp_get(true);
        LOG_INF("Temperature: %8.2f C", temp);
        double pressure = pressure_get();
        LOG_INF("Pressure: %8.2f kPa", pressure);
        k_msleep(1000);
    }
}