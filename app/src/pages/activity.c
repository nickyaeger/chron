#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "../page.h"
#include "activity.h"
#include "summary.h"

/* Settings page functions */
static void on_enter(void);
static void activity_input(ui_button_t button);

/* Activity page definition */
static char name[] = "Activity";
ui_page_t activity = { .on_enter = on_enter, .on_exit = NULL, .handle_input = activity_input, .info = (void*)name };

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(activity, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(activity, LOG_LEVEL_INF);
#endif

static void on_enter(void) {
    LOG_DBG("Current page: ACTIVITY");
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
            break;
        default:
            break;
    }
}
