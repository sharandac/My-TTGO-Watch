/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
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