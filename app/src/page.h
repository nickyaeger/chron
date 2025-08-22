/* Button definition */
typedef enum {
    BTN_PREV,
    BTN_NEXT,
    BTN_BACK,
    BTN_SELECT,
} ui_button_t;

/* Page structure */
typedef struct {
    void (*on_enter)(void);
    void (*on_exit)(void);
    void (*handle_input)(ui_button_t);
    void *info;
} ui_page_t;

/* Workqueue structure */
extern struct k_work_q ui_workq;

/* Work item for each button */
extern struct k_work button0_work;
extern struct k_work button1_work;
extern struct k_work button2_work;
extern struct k_work button3_work;

/* UI stack operations */
ui_page_t *ui_pop(void);
void ui_push(ui_page_t *page);
ui_page_t *ui_reset(void);

/* Routines */
int ui_init(void);
