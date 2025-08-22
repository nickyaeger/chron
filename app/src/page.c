#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "page.h"
#include "pages/home.h"

#define UI_WORKQ_PRIORITY 3
#define UI_STACK_MAX 8
#define UI_WORKQ_THREAD_STACK_SIZE 512

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
static int ui_handle_input(ui_button_t button);
static void offload_prev(struct k_work *work);
static void offload_next(struct k_work *work);
static void offload_back(struct k_work *work);
static void offload_select(struct k_work *work);

/* Register with logger module */
#ifdef CONFIG_DBG_PRINT
    LOG_MODULE_REGISTER(page, LOG_LEVEL_DBG);
#else
    LOG_MODULE_REGISTER(page, LOG_LEVEL_INF);
#endif

/* UI stack operations */
ui_page_t *ui_pop(void) {
    if (ui_stack_top < 1) {
        LOG_ERR("ui_pop failed: Nothing on the UI stack!");
        return NULL;
    }
    if (ui_stack[ui_stack_top]->on_exit) {
        ui_stack[ui_stack_top]->on_exit();
    }
    ui_page_t* page = ui_stack[--ui_stack_top];
    if (ui_stack[ui_stack_top]->on_enter) {
        ui_stack[ui_stack_top]->on_enter();
    }
    return page;

}

void ui_push(ui_page_t *page) {
    if (ui_stack_top >= UI_STACK_MAX - 1) {
        LOG_ERR("ui_push failed: UI stack is full!");
        return;
    }
    if (ui_stack[ui_stack_top]->on_exit) {
        ui_stack[ui_stack_top]->on_exit();
    }
    ui_stack[++ui_stack_top] = page;
    if (ui_stack[ui_stack_top]->on_enter) {
        ui_stack[ui_stack_top]->on_enter();
    }
}

ui_page_t *ui_reset(void) {
    ui_stack_top = 0;
    if (ui_stack[ui_stack_top] == NULL) {
        ui_stack[ui_stack_top] = &home;
    }
    return &home;
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

    ui_reset();
    LOG_INF("UI successfully initialized");
    return 0;
}

/* Button input handler */
static int ui_handle_input(ui_button_t button) {
    LOG_DBG("UI workqueue received button %d press", button);
    ui_stack[ui_stack_top]->handle_input(button);
    return 0;
}
