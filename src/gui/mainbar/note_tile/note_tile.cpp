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

#include "gui/mainbar/mainbar.h"
#include "note_tile.h"

static lv_obj_t *note_cont = NULL;
static lv_obj_t *notelabel = NULL;

static lv_style_t *style;
static lv_style_t notestyle;

LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_16px);

void note_tile_setup( void ) {

    note_cont = mainbar_get_tile_obj( mainbar_add_tile( 0, 1, "note tile" ) );
    style = mainbar_get_style();

    lv_style_copy( &notestyle, style);
    lv_style_set_text_opa( &notestyle, LV_OBJ_PART_MAIN, LV_OPA_30);
    lv_style_set_text_font( &notestyle, LV_STATE_DEFAULT, &Ubuntu_72px);

    notelabel = lv_label_create( note_cont, NULL);
    lv_label_set_text( notelabel, "note");
    lv_obj_reset_style_list( notelabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( notelabel, LV_OBJ_PART_MAIN, &notestyle );
    lv_obj_align( notelabel, NULL, LV_ALIGN_CENTER, 0, 0);
}