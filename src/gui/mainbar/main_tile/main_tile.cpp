#include <stdio.h>
#include <time.h>

#include "config.h"
#include "../mainbar.h"
#include "main_tile.h"

static lv_obj_t *timelabel = NULL;
static lv_obj_t *datelabel = NULL;

static lv_style_t timestyle;
static lv_style_t datestyle;

lv_task_t * task;

LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_16px);

void main_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {

    lv_style_copy( &timestyle, style);
    lv_style_set_text_font( &timestyle, LV_STATE_DEFAULT, &Ubuntu_72px);

    lv_style_copy( &datestyle, style);
    lv_style_set_text_font( &datestyle, LV_STATE_DEFAULT, &Ubuntu_16px);

    /*Tile1: just a label*/
    timelabel = lv_label_create(tile, NULL);
    lv_label_set_text(timelabel, "00:00");
    lv_obj_reset_style_list( timelabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( timelabel, LV_OBJ_PART_MAIN, &timestyle );
    lv_obj_align(timelabel, NULL, LV_ALIGN_CENTER, 0, 0);

    datelabel = lv_label_create(tile, NULL);
    lv_label_set_text(datelabel, "1.Jan 1970");
    lv_obj_reset_style_list( datelabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( datelabel, LV_OBJ_PART_MAIN, &datestyle );
    lv_obj_align(datelabel, timelabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    time_t now;
    struct tm  info;
    char buf[64];

    time(&now);
    localtime_r(&now, &info);

    strftime(buf, sizeof(buf), "%H:%M", &info);
    lv_label_set_text(timelabel, buf);
    strftime(buf, sizeof(buf), "%a %d.%b %Y", &info);
    lv_label_set_text(datelabel, buf);
    lv_obj_align(datelabel, timelabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    task = lv_task_create(main_tile_task, 1000, LV_TASK_PRIO_MID, NULL );
}

/*
 *
 */
void main_tile_task( lv_task_t * task ) {
    time_t now;
    struct tm  info;
    char buf[64];

    time(&now);
    localtime_r(&now, &info);

    strftime(buf, sizeof(buf), "%H:%M", &info);
    lv_label_set_text(timelabel, buf);
    strftime(buf, sizeof(buf), "%a %d.%b %Y", &info);
    lv_label_set_text(datelabel, buf);
    lv_obj_align(datelabel, timelabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}