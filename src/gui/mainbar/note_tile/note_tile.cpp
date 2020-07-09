#include "config.h"
#include "../mainbar.h"
#include "note_tile.h"

static lv_obj_t *notelabel = NULL;

static lv_style_t notestyle;

LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_16px);

void note_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {

    lv_style_copy( &notestyle, style);
    lv_style_set_text_font( &notestyle, LV_STATE_DEFAULT, &Ubuntu_72px);

    notelabel = lv_label_create(tile, NULL);
    lv_label_set_text( notelabel, "note");
    lv_obj_reset_style_list( notelabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( notelabel, LV_OBJ_PART_MAIN, &notestyle );
    lv_obj_align( notelabel, NULL, LV_ALIGN_CENTER, 0, 0);

}