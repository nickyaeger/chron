#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "../page.h"
#include "timer.h"
#include "summary.h"

/* Timer page functions */
static void on_enter(void);
static void timer_input(ui_button_t button);

/* Timer page definition */
static char name[] = "Timer";
ui_page_t timer = { .on_enter = on_enter, .on_exit = NULL, .handle_input = timer_input, .info = (void*)name };

static size_t duration;
static struct k_timer t;

/* Timer expiry callback */
static void timer_done(struct k_timer *t) {
    printk("Timer finished\n");
}

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(timer, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(timer, LOG_LEVEL_INF);
#endif

static void on_enter(void) {
    LOG_DBG("Current page: TIMER");
}

int timer_init(void) {
    k_timer_init(&t, timer_done, NULL);
    duration = 0;
    LOG_INF("Timer successfully initialized");
    return 0;
}

/* Timer page input handler */
static void timer_input(ui_button_t button) {
    switch (button) {
        case BTN_PREV:
            k_ticks_t remaining = k_timer_remaining_ticks(&t);
            if (remaining > 0) {
                k_timer_stop(&t);
                LOG_INF("Timer stopped");
            } else if (duration > 0) {
                duration -= 30;
                LOG_INF("Timer duration set to %d seconds", duration);
            }
            break;
        case BTN_NEXT:
            remaining = k_timer_remaining_ticks(&t);
            if (remaining > 0) {
                k_timer_stop(&t);
                LOG_INF("Timer stopped");
            } else if (duration < 3600) {
                duration += 30;
                LOG_INF("Timer duration set to %d seconds", duration);
            }
            break;
        case BTN_BACK:
            set_carousel_view(TIMER_VIEW);
            ui_pop();
            break;
        case BTN_SELECT:
            remaining = k_timer_remaining_ticks(&t);
            if (remaining == 0) {
                if (duration > 0) {
                    k_timer_start(&t, K_SECONDS(duration), K_FOREVER);
                    LOG_INF("Started timer for %d seconds", duration);
                } else {
                    LOG_INF("Cannot start timer with zero duration");
                }
            } else {
                LOG_INF("A timer is already running");
            }
            break;
        default:
            break;
    }
}
