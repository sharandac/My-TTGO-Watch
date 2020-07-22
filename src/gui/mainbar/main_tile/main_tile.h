#ifndef _MAIL_TILE_H
    #define _MAIL_TILE_H

    #include <TTGO.h>

    #define MAX_WIDGET_NUM  3

    typedef struct {
        lv_obj_t *widget;
        lv_coord_t x;
        lv_coord_t y;
        bool active;
    } lv_widget_entry_t;

    void main_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );
    void main_tile_task( lv_task_t * task );
    lv_obj_t *main_tile_register_widget( void );

#endif // _MAIL_TILE_H