/****************************************************************************
 *   June 14 02:01:00 2021
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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

#include "kodi_remote_app.h"
#include "kodi_remote_app_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>

    using namespace std;
    #define String string
#else
    #include <Arduino.h>
    #include <FS.h>
    #include <SPIFFS.h>
#endif

lv_obj_t *kodi_remote_app_setup_tile = NULL;
lv_obj_t *kodi_remote_host_textfield = NULL;
lv_obj_t *kodi_remote_port_textfield = NULL;
lv_obj_t *kodi_remote_user_textfield = NULL;
lv_obj_t *kodi_remote_pass_textfield = NULL;

static void exit_kodi_remote_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_setup_hibernate_callback ( void );
void kodi_remote_setup_save_config( void );

void kodi_remote_app_setup_setup( uint32_t tile_num ) {
    kodi_remote_config_t *kodi_remote_config = kodi_remote_get_config();

    mainbar_add_tile_hibernate_cb( tile_num, kodi_remote_setup_hibernate_callback );

    kodi_remote_app_setup_tile = mainbar_get_tile_obj( tile_num );
    
    lv_obj_t *header = wf_add_settings_header( kodi_remote_app_setup_tile, "kodi remote setup", exit_kodi_remote_app_setup_event_cb );
    lv_obj_align( header, kodi_remote_app_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );

    // config fields    
    lv_obj_t *kodi_remote_server_cont = lv_obj_create( kodi_remote_app_setup_tile, NULL );
    lv_obj_set_size( kodi_remote_server_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( kodi_remote_server_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( kodi_remote_server_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );
    lv_obj_t *kodi_remote_server_label = lv_label_create( kodi_remote_server_cont, NULL);
    lv_obj_add_style( kodi_remote_server_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( kodi_remote_server_label, "host");
    lv_obj_align( kodi_remote_server_label, kodi_remote_server_cont, LV_ALIGN_IN_LEFT_MID, THEME_ICON_PADDING, 0 );
    kodi_remote_host_textfield = lv_textarea_create( kodi_remote_server_cont, NULL);
    lv_textarea_set_text( kodi_remote_host_textfield, kodi_remote_config->host );
    lv_textarea_set_pwd_mode( kodi_remote_host_textfield, false);
    lv_textarea_set_one_line( kodi_remote_host_textfield, true);
    lv_textarea_set_cursor_hidden( kodi_remote_host_textfield, true);
    lv_obj_set_width( kodi_remote_host_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - THEME_ICON_PADDING );
    lv_obj_align( kodi_remote_host_textfield, kodi_remote_server_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_ICON_PADDING, 0 );
    lv_obj_set_event_cb( kodi_remote_host_textfield, kodi_remote_textarea_event_cb );

    lv_obj_t *kodi_remote_port_cont = lv_obj_create( kodi_remote_app_setup_tile, NULL );
    lv_obj_set_size( kodi_remote_port_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( kodi_remote_port_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( kodi_remote_port_cont, kodi_remote_server_cont, LV_ALIGN_OUT_BOTTOM_MID, 0,  THEME_ICON_PADDING );
    lv_obj_t *kodi_remote_port_label = lv_label_create( kodi_remote_port_cont, NULL);
    lv_obj_add_style( kodi_remote_port_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( kodi_remote_port_label, "port");
    lv_obj_align( kodi_remote_port_label, kodi_remote_port_cont, LV_ALIGN_IN_LEFT_MID, THEME_ICON_PADDING, 0 );
    kodi_remote_port_textfield = lv_textarea_create( kodi_remote_port_cont, NULL);
    char buf[10];
    sprintf(buf, "%d", kodi_remote_config->port );
    lv_textarea_set_text( kodi_remote_port_textfield, buf);
    lv_textarea_set_pwd_mode( kodi_remote_port_textfield, false);
    lv_textarea_set_one_line( kodi_remote_port_textfield, true);
    lv_textarea_set_cursor_hidden( kodi_remote_port_textfield, true);
    lv_obj_set_width( kodi_remote_port_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - THEME_ICON_PADDING );
    lv_obj_align( kodi_remote_port_textfield, kodi_remote_port_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_ICON_PADDING, 0 );
    lv_obj_set_event_cb( kodi_remote_port_textfield, kodi_remote_num_textarea_event_cb );

    lv_obj_t *kodi_remote_user_cont = lv_obj_create( kodi_remote_app_setup_tile, NULL );
    lv_obj_set_size( kodi_remote_user_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( kodi_remote_user_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( kodi_remote_user_cont, kodi_remote_port_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );
    lv_obj_t *kodi_remote_user_label = lv_label_create( kodi_remote_user_cont, NULL);
    lv_obj_add_style( kodi_remote_user_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( kodi_remote_user_label, "user");
    lv_obj_align( kodi_remote_user_label, kodi_remote_user_cont, LV_ALIGN_IN_LEFT_MID, THEME_ICON_PADDING, 0 );
    kodi_remote_user_textfield = lv_textarea_create( kodi_remote_user_cont, NULL);
    lv_textarea_set_text( kodi_remote_user_textfield, kodi_remote_config->user );
    lv_textarea_set_pwd_mode( kodi_remote_user_textfield, false);
    lv_textarea_set_one_line( kodi_remote_user_textfield, true);
    lv_textarea_set_cursor_hidden( kodi_remote_user_textfield, true);
    lv_obj_set_width( kodi_remote_user_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - THEME_ICON_PADDING );
    lv_obj_align( kodi_remote_user_textfield, kodi_remote_user_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_ICON_PADDING, 0 );
    lv_obj_set_event_cb( kodi_remote_user_textfield, kodi_remote_textarea_event_cb );

    lv_obj_t *kodi_remote_password_cont = lv_obj_create( kodi_remote_app_setup_tile, NULL );
    lv_obj_set_size( kodi_remote_password_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( kodi_remote_password_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( kodi_remote_password_cont, kodi_remote_user_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );
    lv_obj_t *kodi_remote_password_label = lv_label_create( kodi_remote_password_cont, NULL);
    lv_obj_add_style( kodi_remote_password_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( kodi_remote_password_label, "pass");
    lv_obj_align( kodi_remote_password_label, kodi_remote_password_cont, LV_ALIGN_IN_LEFT_MID, THEME_ICON_PADDING, 0 );
    kodi_remote_pass_textfield = lv_textarea_create( kodi_remote_password_cont, NULL);
    lv_textarea_set_text( kodi_remote_pass_textfield, kodi_remote_config->pass );
    lv_textarea_set_pwd_mode( kodi_remote_pass_textfield, false);
    lv_textarea_set_one_line( kodi_remote_pass_textfield, true);
    lv_textarea_set_cursor_hidden( kodi_remote_pass_textfield, true);
    lv_obj_set_width( kodi_remote_pass_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - THEME_ICON_PADDING );
    lv_obj_align( kodi_remote_pass_textfield, kodi_remote_password_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_ICON_PADDING, 0 );
    lv_obj_set_event_cb( kodi_remote_pass_textfield, kodi_remote_textarea_event_cb );
    
    lv_tileview_add_element( kodi_remote_app_setup_tile, kodi_remote_server_cont );
    lv_tileview_add_element( kodi_remote_app_setup_tile, kodi_remote_port_cont );
    lv_tileview_add_element( kodi_remote_app_setup_tile, kodi_remote_user_cont );
    lv_tileview_add_element( kodi_remote_app_setup_tile, kodi_remote_password_cont );
}

static void exit_kodi_remote_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        kodi_remote_setup_save_config();
                                        break;
    }
}

static void kodi_remote_setup_hibernate_callback ( void ) {
    keyboard_hide();
    kodi_remote_setup_save_config();
}

static void kodi_remote_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void kodi_remote_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}

void kodi_remote_setup_save_config( void ) {
    kodi_remote_config_t *kodi_remote_config = kodi_remote_get_config();
    strncpy( kodi_remote_config->host, lv_textarea_get_text( kodi_remote_host_textfield ), sizeof( kodi_remote_config->host ) );
    strncpy( kodi_remote_config->user, lv_textarea_get_text( kodi_remote_user_textfield ), sizeof( kodi_remote_config->user ) );
    strncpy( kodi_remote_config->pass, lv_textarea_get_text( kodi_remote_pass_textfield ), sizeof( kodi_remote_config->pass ) );
    kodi_remote_config->port = atoi(lv_textarea_get_text( kodi_remote_port_textfield ));
    kodi_remote_save_config();
}