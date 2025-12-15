/* Summary page structure declaration */
extern ui_page_t summary;

/* Carousel definition */
typedef enum {
    SETTINGS_VIEW,
    ACTIVITY_VIEW,
    CLIMATE_VIEW,
} carousel_view_t;

/**
 * @brief Sets a particular view 
 *
 * The carousel contains "previews" for a number of pages such that quick access 
 * to information from different pages is simplified.
 *
 */
void set_carousel_view(carousel_view_t view);
