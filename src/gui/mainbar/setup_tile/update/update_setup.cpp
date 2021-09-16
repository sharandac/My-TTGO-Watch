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
#include "config/updateconfig.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

update_config_t update_config;

lv_obj_t *update_setup_tile = NULL;                 /** @brief pointer to the setup tile object */
uint32_t update_setup_tile_num;                     /** @brief setup tile number */

lv_obj_t *update_check_autosync_onoff = NULL;       /** @brief pointer to autosync switch object */
lv_obj_t *update_check_autorestart_onoff = NULL;    /** @brief pointer to autorestart switch object */
lv_obj_t *update_check_url_textfield = NULL;        /** @brief pointer to url textarea object */

LV_IMG_DECLARE(setup_32px);

static void update_check_url_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void update_reset_url_event_cb( lv_obj_t * obj, lv_event_t event );
static void update_check_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
static void update_check_autorestart_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
static void exit_update_check_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void update_setup_tile_setup( uint32_t tile_num ) {

    update_config.load();

    update_setup_tile_num = tile_num;
    update_setup_tile = mainbar_get_tile_obj( update_setup_tile_num );

    lv_obj_add_style( update_setup_tile, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );

    lv_obj_t *header = wf_add_settings_header( update_setup_tile, "update settings", exit_update_check_setup_event_cb );
    lv_obj_align( header, update_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );

    lv_obj_t *update_check_autorestart_cont = wf_add_labeled_switch( update_setup_tile, "reset after update", &update_check_autorestart_onoff, update_config.autorestart, update_check_autorestart_onoff_event_handler );
    lv_obj_align( update_check_autorestart_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    lv_obj_t *update_check_autosync_cont = wf_add_labeled_switch( update_setup_tile, "check for updates", &update_check_autosync_onoff, update_config.autosync, update_check_autosync_onoff_event_handler );
    lv_obj_align( update_check_autosync_cont, update_check_autorestart_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    lv_obj_t *update_check_url_cont = lv_obj_create( update_setup_tile, NULL );
    lv_obj_set_size(update_check_url_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( update_check_url_cont, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_obj_align( update_check_url_cont, update_check_autosync_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    
    update_check_url_textfield = lv_textarea_create( update_check_url_cont, NULL);
    lv_textarea_set_text( update_check_url_textfield, update_config.updateurl );
    lv_textarea_set_pwd_mode( update_check_url_textfield, false);
    lv_textarea_set_one_line( update_check_url_textfield, true);
    lv_textarea_set_cursor_hidden( update_check_url_textfield, true);
    lv_obj_set_width( update_check_url_textfield, lv_disp_get_hor_res( NULL ) - 10 );
    lv_obj_align( update_check_url_textfield, update_check_url_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_obj_set_event_cb( update_check_url_textfield, update_check_url_textarea_event_cb );

    lv_obj_t *update_reset_url_btn = lv_btn_create( update_setup_tile, NULL);
    lv_obj_set_event_cb( update_reset_url_btn, update_reset_url_event_cb );
    lv_obj_add_style( update_reset_url_btn, LV_BTN_PART_MAIN, ws_get_button_style() );
    lv_obj_align( update_reset_url_btn, update_check_url_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_t *update_reset_url_label = lv_label_create( update_reset_url_btn, NULL );
    lv_label_set_text( update_reset_url_label, "set default url");
}

/**
 * @brief update url callback function
 */
static void update_check_url_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

/**
 * @brief update reset url string button callback function
 */
static void update_reset_url_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        lv_textarea_set_text( update_check_url_textfield, FIRMWARE_UPDATE_URL );
    }
}

/**
 * @brief update autosync switch callback function
 */
static void update_check_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case ( LV_EVENT_VALUE_CHANGED ):    update_config.autosync = lv_switch_get_state( obj );
                                            break;
    }
}

/**
 * @brief update autoreset switch callback function
 */
static void update_check_autorestart_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case ( LV_EVENT_VALUE_CHANGED ):    update_config.autorestart = lv_switch_get_state( obj );
                                            break;
    }
}

/**
 * @brief exit button callback function
 */
static void exit_update_check_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):           mainbar_jump_back();
                                            strncpy( update_config.updateurl , lv_textarea_get_text( update_check_url_textfield ), FIRMWARE_UPDATE_URL_LEN );
                                            update_config.save();
                                            break;
    }
}

bool update_setup_get_autosync( void ) {
    return( update_config.autosync );
}

bool update_setup_get_autorestart( void ) {
    return( update_config.autorestart );
}

char* update_setup_get_url( void ) {
    return( update_config.updateurl );
}
