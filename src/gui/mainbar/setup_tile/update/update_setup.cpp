/****************************************************************************
 *   July 31 07:31:12 2020
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
#include <config.h>

#include "update.h"
#include "update_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"

#include "hardware/motor.h"

update_config_t update_config;

lv_obj_t *update_setup_tile = NULL;
lv_style_t update_setup_style;
uint32_t update_setup_tile_num;

lv_obj_t *update_check_autosync_onoff = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);

static void exit_update_check_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void update_check_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
void update_read_config( void );
void update_save_config( void );

void update_setup_tile_setup( uint32_t tile_num ) {

    update_read_config();

    update_setup_tile_num = tile_num;
    update_setup_tile = mainbar_get_tile_obj( update_setup_tile_num );

    lv_style_copy( &update_setup_style, mainbar_get_style() );
    lv_style_set_bg_color( &update_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &update_setup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &update_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( update_setup_tile, LV_OBJ_PART_MAIN, &update_setup_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( update_setup_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &update_setup_style );
    lv_obj_align( exit_btn, update_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_update_check_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( update_setup_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &update_setup_style  );
    lv_label_set_text( exit_label, "update settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *update_check_autosync_cont = lv_obj_create( update_setup_tile, NULL );
    lv_obj_set_size(update_check_autosync_cont, LV_HOR_RES_MAX , 40);
    lv_obj_add_style( update_check_autosync_cont, LV_OBJ_PART_MAIN, &update_setup_style  );
    lv_obj_align( update_check_autosync_cont, update_setup_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );

    update_check_autosync_onoff = lv_switch_create( update_check_autosync_cont, NULL );
    lv_obj_add_protect( update_check_autosync_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( update_check_autosync_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( update_check_autosync_onoff, LV_ANIM_ON );
    lv_obj_align( update_check_autosync_onoff, update_check_autosync_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( update_check_autosync_onoff, update_check_autosync_onoff_event_handler );
    lv_obj_t *update_check_autosync_label = lv_label_create( update_check_autosync_cont, NULL);
    lv_obj_add_style( update_check_autosync_label, LV_OBJ_PART_MAIN, &update_setup_style  );
    lv_label_set_text( update_check_autosync_label, "check for updates");
    lv_obj_align( update_check_autosync_label, update_check_autosync_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    if ( update_config.autosync)
        lv_switch_on( update_check_autosync_onoff, LV_ANIM_OFF);
    else
        lv_switch_off( update_check_autosync_onoff, LV_ANIM_OFF);

}


static void update_check_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case (LV_EVENT_VALUE_CHANGED):      update_config.autosync = lv_switch_get_state( obj );
                                            update_save_config();
                                            break;
    }
}

static void exit_update_check_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):           mainbar_jump_to_tilenumber( update_setup_tile_num - 1, false );
                                            break;
    }
}

void update_save_config( void ) {
  fs::File file = SPIFFS.open( UPDATE_CONFIG_FILE, FILE_WRITE );

  if ( !file ) {
    log_e("Can't save file: %s", UPDATE_CONFIG_FILE );
  }
  else {
    file.write( (uint8_t *)&update_config, sizeof( update_config ) );
    file.close();
  }
}

void update_read_config( void ) {
  fs::File file = SPIFFS.open( UPDATE_CONFIG_FILE, FILE_READ );

  if (!file) {
    log_e("Can't open file: %s!", UPDATE_CONFIG_FILE );
  }
  else {
    int filesize = file.size();
    if ( filesize > sizeof( update_config ) ) {
      log_e("Failed to read configfile. Wrong filesize!" );
    }
    else {
      file.read( (uint8_t *)&update_config, filesize );
    }
    file.close();
  }
}

bool update_setup_get_autosync( void ) {
    return( update_config.autosync );
}
