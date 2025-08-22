#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "../page.h"
#include "home.h"
#include "summary.h"

/* Home page functions */
static void on_enter(void);
static void home_input(ui_button_t button);

/* Home page definition */
static char name[] = "Home";
ui_page_t home = { .on_enter = on_enter, .on_exit = NULL, .handle_input = home_input, .info = (void*)name };

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(home, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(home, LOG_LEVEL_INF);
#endif

static void on_enter(void) {
    LOG_DBG("Current page: HOME");
}

/* Home page input handler */
static void home_input(ui_button_t button) {
    switch (button) {
        case BTN_PREV:
            set_carousel_view(ACTIVITY_VIEW);
            ui_push(&summary);
            break;
        case BTN_NEXT:
            set_carousel_view(SETTINGS_VIEW);
            ui_push(&summary);
            break;
        case BTN_BACK:
            break;
        case BTN_SELECT:
            break;
        default:
            break;
    }
}
