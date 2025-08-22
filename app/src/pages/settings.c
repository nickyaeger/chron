#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "../page.h"
#include "settings.h"
#include "summary.h"

/* Settings page functions */
static void on_enter(void);
static void settings_input(ui_button_t button);

/* Settings page definition */
static char name[] = "Settings";
ui_page_t settings = { .on_enter = on_enter, .on_exit = NULL, .handle_input = settings_input, .info = (void*)name };

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(settings, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(settings, LOG_LEVEL_INF);
#endif

static void on_enter(void) {
    LOG_DBG("Current page: SETTINGS");
}

/* Settings page input handler */
static void settings_input(ui_button_t button) {
    switch (button) {
        case BTN_PREV:
            break;
        case BTN_NEXT:
            break;
        case BTN_BACK:
            set_carousel_view(SETTINGS_VIEW);
            ui_pop();
            break;
        case BTN_SELECT:
            break;
        default:
            break;
    }
}
