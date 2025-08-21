#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "page.h"

#define UI_WORKQ_PRIORITY 3
#define UI_STACK_MAX 8
#define UI_WORKQ_THREAD_STACK_SIZE 512

typedef enum {
    BTN_PREV,
    BTN_NEXT,
    BTN_BACK,
    BTN_SELECT,
} ui_button_t;

/* Page structure */
typedef struct {
    void (*render)(void);
    void (*handle_input)(ui_button_t);
    void *info;
} ui_page_t;

/* Define UI stack */
static ui_page_t *ui_stack[UI_STACK_MAX] = {0};
static int ui_stack_top = -1;

/* Define workqueue structure */
struct k_work_q ui_workq = {0};

/* Define stack area used by workqueue thread */
static K_THREAD_STACK_DEFINE(my_stack_area, UI_WORKQ_THREAD_STACK_SIZE);

/* Work item for each button */
struct k_work button0_work;
struct k_work button1_work;
struct k_work button2_work;
struct k_work button3_work;

/* Function prototypes */
static ui_page_t *ui_pop(void);
static void ui_push(ui_page_t *page);
static void home_input(ui_button_t button);
static void settings_input(ui_button_t button);
int ui_handle_input(ui_button_t button);

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(page, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(page, LOG_LEVEL_INF);
#endif

/* Page definitions */

/* Home page */
static ui_page_t home = { .render = NULL, .handle_input = home_input };
/* Settings page */
static ui_page_t settings = { .render = NULL, .handle_input = settings_input };

/* Page input handler definitions */

/* Home page input handler */
static void home_input(ui_button_t button) {
    switch (button) {
        case BTN_PREV:
            ui_push(&settings);
            break;
        case BTN_NEXT:
            ui_push(&settings);
            break;
        case BTN_BACK:
            break;
        case BTN_SELECT:
            break;
        default:
            break;
    }
}

/* Settings page input handler */
static void settings_input(ui_button_t button) {
    switch (button) {
        case BTN_PREV:
            ui_pop();
            break;
        case BTN_NEXT:
            ui_pop();
            break;
        case BTN_BACK:
            ui_pop();
            break;
        case BTN_SELECT:
            break;
        default:
            break;
    }
}

/* UI stack operations */
static ui_page_t *ui_pop(void) {
    if (ui_stack_top < 1) {
        LOG_ERR("ui_pop failed: Nothing on the UI stack!");
        return NULL;
    }
    return ui_stack[--ui_stack_top];

}

static void ui_push(ui_page_t *page) {
    if (ui_stack_top >= UI_STACK_MAX - 1) {
        LOG_ERR("ui_push failed: UI stack is full!");
        return;
    }
    ui_stack[++ui_stack_top] = page;
}

/* Workqueue offload functions */
static void offload_prev(struct k_work *work) {
    ui_handle_input(BTN_PREV);
}
static void offload_next(struct k_work *work) {
    ui_handle_input(BTN_NEXT);
}
static void offload_back(struct k_work *work) {
    ui_handle_input(BTN_BACK);
}
static void offload_select(struct k_work *work) {
    ui_handle_input(BTN_SELECT);
}

/* Must be called before buttons can be used */
int ui_init(void) {
    k_work_queue_start(&ui_workq, my_stack_area, UI_WORKQ_THREAD_STACK_SIZE, UI_WORKQ_PRIORITY, NULL);
    k_work_init(&button0_work, offload_prev);
    k_work_init(&button1_work, offload_next);
    k_work_init(&button2_work, offload_back);
    k_work_init(&button3_work, offload_select);
    
    char *home_str = "Home page";
    char *settings_str = "Settings page";
    home.info = (void *)home_str;
    settings.info = (void *)settings_str;

    ui_push(&home);
    return 0;
}

/* Prototype for button input handler */
int ui_handle_input(ui_button_t button) {
    LOG_DBG("UI workqueue received button %d press", button);
    ui_stack[ui_stack_top]->handle_input(button);
    LOG_DBG("Current page: %s", (char *)(ui_stack[ui_stack_top]->info));
    return 0;
}
