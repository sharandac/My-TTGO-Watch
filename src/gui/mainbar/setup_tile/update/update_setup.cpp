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

#include "hardware/json_psram_allocator.h"
#include "hardware/alloc.h"

static update_config_t *update_config = NULL;

lv_obj_t *update_setup_tile = NULL;
lv_style_t update_setup_style;
uint32_t update_setup_tile_num;

lv_obj_t *update_check_autosync_onoff = NULL;
lv_obj_t *update_check_url_textfield = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);

static void update_check_url_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void update_reset_url_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_update_check_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void update_check_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
void update_read_config( void );
void update_save_config( void );

void update_setup_tile_setup( uint32_t tile_num ) {

    update_config = (update_config_t*)CALLOC( sizeof( update_config_t ), 1 );
    if( !update_config ) {
      log_e("update_config calloc faild");
      while(true);
    }

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
    lv_obj_set_size(update_check_autosync_cont, lv_disp_get_hor_res( NULL ) , 40);
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

    lv_obj_t *update_check_url_cont = lv_obj_create( update_setup_tile, NULL );
    lv_obj_set_size(update_check_url_cont, lv_disp_get_hor_res( NULL ) , 60);
    lv_obj_add_style( update_check_url_cont, LV_OBJ_PART_MAIN, &update_setup_style  );
    lv_obj_align( update_check_url_cont, update_check_autosync_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *update_check_url_label = lv_label_create( update_check_url_cont, NULL);
    lv_obj_add_style( update_check_url_label, LV_OBJ_PART_MAIN, &update_setup_style  );
    lv_label_set_text( update_check_url_label, "update URL");
    lv_obj_align( update_check_url_label, update_check_url_cont, LV_ALIGN_IN_TOP_LEFT, 5, 0 );
    update_check_url_textfield = lv_textarea_create( update_check_url_cont, NULL);
    lv_textarea_set_text( update_check_url_textfield, update_config->updateurl );
    lv_textarea_set_pwd_mode( update_check_url_textfield, false);
    lv_textarea_set_one_line( update_check_url_textfield, true);
    lv_textarea_set_cursor_hidden( update_check_url_textfield, true);
    lv_obj_set_width( update_check_url_textfield, lv_disp_get_hor_res( NULL ) - 10 );
    lv_obj_align( update_check_url_textfield, update_check_url_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_obj_set_event_cb( update_check_url_textfield, update_check_url_textarea_event_cb );

    lv_obj_t *update_reset_url_btn = lv_btn_create( update_setup_tile, NULL);
    lv_obj_set_event_cb( update_reset_url_btn, update_reset_url_event_cb );
    lv_obj_add_style( update_reset_url_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( update_reset_url_btn, update_check_url_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_t *update_reset_url_label = lv_label_create( update_reset_url_btn, NULL );
    lv_label_set_text( update_reset_url_label, "set default url");

    if ( update_config->autosync )
        lv_switch_on( update_check_autosync_onoff, LV_ANIM_OFF);
    else
        lv_switch_off( update_check_autosync_onoff, LV_ANIM_OFF);

}

static void update_check_url_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void update_reset_url_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        lv_textarea_set_text( update_check_url_textfield, FIRMWARE_UPDATE_URL );
    }
}

static void update_check_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case ( LV_EVENT_VALUE_CHANGED ):    update_config->autosync = lv_switch_get_state( obj );
                                            break;
    }
}

static void exit_update_check_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):           mainbar_jump_to_tilenumber( update_setup_tile_num - 1, false );
                                            strlcpy( update_config->updateurl , lv_textarea_get_text( update_check_url_textfield ), sizeof( update_config->updateurl ) );
                                            update_save_config();
                                            break;
    }
}

void update_save_config( void ) {
    if ( SPIFFS.exists( UPDATE_CONFIG_FILE ) ) {
        SPIFFS.remove( UPDATE_CONFIG_FILE );
        log_i("remove old binary update config");
    }

    fs::File file = SPIFFS.open( UPDATE_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", UPDATE_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["autosync"] = update_config->autosync;
        doc["updateurl"] = update_config->updateurl;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void update_read_config( void ) {
    if ( SPIFFS.exists( UPDATE_JSON_CONFIG_FILE ) ) {       
        fs::File file = SPIFFS.open( UPDATE_JSON_CONFIG_FILE, FILE_READ );
        if (!file) {
            log_e("Can't open file: %s!", UPDATE_JSON_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            SpiRamJsonDocument doc( filesize * 2 );

            DeserializationError error = deserializeJson( doc, file );
            if ( error ) {
                log_e("update check deserializeJson() failed: %s", error.c_str() );
            }
            else {
                update_config->autosync = doc["autosync"].as<bool>();
                strlcpy( update_config->updateurl, doc["updateurl"] | FIRMWARE_UPDATE_URL, sizeof( update_config->updateurl ) );
            }        
            doc.clear();
        }
        file.close();
    }
    else {
        log_i("no json config exists, read from binary");
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
                file.close();
                update_save_config();
                return; 
            }
        file.close();
        }
    }
}

bool update_setup_get_autosync( void ) {
    return( update_config->autosync );
}

char* update_setup_get_url( void ) {
    return( update_config->updateurl );
}