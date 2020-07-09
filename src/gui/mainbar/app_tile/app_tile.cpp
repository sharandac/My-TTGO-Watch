#include "config.h"
#include "../mainbar.h"
#include "app_tile.h"

static lv_obj_t *applabel = NULL;

static lv_style_t appstyle;

LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_16px);

void app_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {

    lv_style_copy( &appstyle, style);
    lv_style_set_text_font( &appstyle, LV_STATE_DEFAULT, &Ubuntu_72px);

    applabel = lv_label_create(tile, NULL);
    lv_label_set_text(applabel, "app");
    lv_obj_reset_style_list( applabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( applabel, LV_OBJ_PART_MAIN, &appstyle );
    lv_obj_align(applabel, NULL, LV_ALIGN_CENTER, 0, 0);
}
