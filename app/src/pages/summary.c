#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "../page.h"
#include "summary.h"
#include "home.h"
#include "settings.h"
#include "activity.h"

/* Summary page functions */
static void on_enter(void);
static void summary_input(ui_button_t button);

/* Summary page definition */
static char name[] = "Summary";
ui_page_t summary = { .on_enter = on_enter, .on_exit = NULL, .handle_input = summary_input, .info = (void*)name };

/* Currently selected preview in carousel */
static int selected_view = SETTINGS_VIEW;
/* Number of previews */
#define NUM_VIEWS 2

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(summary, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(summary, LOG_LEVEL_INF);
#endif

static void on_enter(void) {
    LOG_DBG("Current page: SUMMARY[%d]", selected_view);
}

void set_carousel_view(carousel_view_t view) {
    selected_view = view;
}

/* Home page input handler */
static void summary_input(ui_button_t button) {
    switch (button) {
        case BTN_PREV:
            if (selected_view == SETTINGS_VIEW) {
                ui_pop();
                break;
            }
            selected_view = (selected_view - 1) % NUM_VIEWS;
            break;
        case BTN_NEXT:
            if (selected_view == ACTIVITY_VIEW) {
                ui_pop();
                break;
            }
            selected_view = (selected_view + 1) % NUM_VIEWS;
            break;
        case BTN_BACK:
            ui_pop();
            break;
        case BTN_SELECT:
            switch (selected_view) {
                case SETTINGS_VIEW:
                    ui_push(&settings);
                    break;
                case ACTIVITY_VIEW:
                    ui_push(&activity);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}
