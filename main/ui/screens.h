#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *header_panel;
    lv_obj_t *obj0;
    lv_obj_t *temperature_panel;
    lv_obj_t *temp_header_panel;
    lv_obj_t *humid_panel;
    lv_obj_t *temp_header_panel_1;
    lv_obj_t *humid_panel_1;
    lv_obj_t *temp_header_panel_2;
    lv_obj_t *temperature_bar;
    lv_obj_t *temperatur_value_label;
    lv_obj_t *temperatur_label;
    lv_obj_t *humidity_label;
    lv_obj_t *humidity_value_label;
    lv_obj_t *pressure_label;
    lv_obj_t *pressure_value_label;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/