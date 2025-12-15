#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "../imu.h"
#include "../page.h"
#include "activity.h"
#include "summary.h"

#define STACKSIZE 1024
#define STEP_COUNTER_PRIORITY 7

/* Activity page functions */
static void on_enter(void);
static void on_exit(void);
static void activity_input(ui_button_t button);

/* Activity page threads */
static void step_counter(void);
K_THREAD_DEFINE(step_counter_id, STACKSIZE, step_counter, NULL, NULL, NULL, STEP_COUNTER_PRIORITY, 0, SYS_FOREVER_MS);

/* Activity page definition */
static char name[] = "Activity";
ui_page_t activity = { .on_enter = on_enter, .on_exit = on_exit, .handle_input = activity_input, .info = (void*)name };

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(activity, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(activity, LOG_LEVEL_INF);
#endif

static void on_enter(void) {
    LOG_DBG("Current page: ACTIVITY");
    k_thread_start(step_counter_id);
    k_thread_resume(step_counter_id);
}

static void on_exit(void) {
    k_thread_suspend(step_counter_id);
}

/* Activity page input handler */
static void activity_input(ui_button_t button) {
    switch (button) {
        case BTN_PREV:
            break;
        case BTN_NEXT:
            break;
        case BTN_BACK:
            set_carousel_view(ACTIVITY_VIEW);
            ui_pop();
            break;
        case BTN_SELECT:
            steps_reset();
            LOG_INF("Reset step counter");
            break;
        default:
            break;
    }
}

/* Step counter thread */
static void step_counter(void) {
    while (1) {
        uint16_t steps = steps_get();
        LOG_INF("Steps: %d", steps);
        k_msleep(1000);
    }
}
