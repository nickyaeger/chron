/* Workqueue structure */
extern struct k_work_q ui_workq;

/* Work item for each button */
extern struct k_work button0_work;
extern struct k_work button1_work;
extern struct k_work button2_work;
extern struct k_work button3_work;

/* Routines */
int ui_init(void);
