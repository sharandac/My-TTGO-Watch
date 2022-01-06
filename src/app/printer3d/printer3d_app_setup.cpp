/****************************************************************************
 *   January 04 19:00:00 2022
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

#include "printer3d_app.h"
#include "printer3d_app_setup.h"

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

lv_obj_t *printer3d_app_setup_tile = NULL;
lv_obj_t *printer3d_host_textfield = NULL;
lv_obj_t *printer3d_port_textfield = NULL;

static void exit_printer3d_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void printer3d_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void printer3d_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void printer3d_setup_hibernate_callback ( void );
void printer3d_setup_save_config( void );

void printer3d_app_setup_setup( uint32_t tile_num ) {
    printer3d_config_t *printer3d_config = printer3d_get_config();

    mainbar_add_tile_hibernate_cb( tile_num, printer3d_setup_hibernate_callback );

    printer3d_app_setup_tile = mainbar_get_tile_obj( tile_num );
    
    lv_obj_t *header = wf_add_settings_header( printer3d_app_setup_tile, "3d printer setup", exit_printer3d_app_setup_event_cb );
    lv_obj_align( header, printer3d_app_setup_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );

    // config fields    
    lv_obj_t *printer3d_server_cont = lv_obj_create( printer3d_app_setup_tile, NULL );
    lv_obj_set_size( printer3d_server_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( printer3d_server_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( printer3d_server_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );
    lv_obj_t *printer3d_server_label = lv_label_create( printer3d_server_cont, NULL);
    lv_obj_add_style( printer3d_server_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( printer3d_server_label, "host");
    lv_obj_align( printer3d_server_label, printer3d_server_cont, LV_ALIGN_IN_LEFT_MID, THEME_ICON_PADDING, 0 );
    printer3d_host_textfield = lv_textarea_create( printer3d_server_cont, NULL);
    lv_textarea_set_text( printer3d_host_textfield, printer3d_config->host );
    lv_textarea_set_pwd_mode( printer3d_host_textfield, false);
    lv_textarea_set_one_line( printer3d_host_textfield, true);
    lv_textarea_set_cursor_hidden( printer3d_host_textfield, true);
    lv_obj_set_width( printer3d_host_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - THEME_ICON_PADDING );
    lv_obj_align( printer3d_host_textfield, printer3d_server_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_ICON_PADDING, 0 );
    lv_obj_set_event_cb( printer3d_host_textfield, printer3d_textarea_event_cb );

    lv_obj_t *printer3d_port_cont = lv_obj_create( printer3d_app_setup_tile, NULL );
    lv_obj_set_size( printer3d_port_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( printer3d_port_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( printer3d_port_cont, printer3d_server_cont, LV_ALIGN_OUT_BOTTOM_MID, 0,  THEME_ICON_PADDING );
    lv_obj_t *printer3d_port_label = lv_label_create( printer3d_port_cont, NULL);
    lv_obj_add_style( printer3d_port_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( printer3d_port_label, "port");
    lv_obj_align( printer3d_port_label, printer3d_port_cont, LV_ALIGN_IN_LEFT_MID, THEME_ICON_PADDING, 0 );
    printer3d_port_textfield = lv_textarea_create( printer3d_port_cont, NULL);
    char buf[10];
    sprintf(buf, "%d", printer3d_config->port );
    lv_textarea_set_text( printer3d_port_textfield, buf);
    lv_textarea_set_pwd_mode( printer3d_port_textfield, false);
    lv_textarea_set_one_line( printer3d_port_textfield, true);
    lv_textarea_set_cursor_hidden( printer3d_port_textfield, true);
    lv_obj_set_width( printer3d_port_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - THEME_ICON_PADDING );
    lv_obj_align( printer3d_port_textfield, printer3d_port_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_ICON_PADDING, 0 );
    lv_obj_set_event_cb( printer3d_port_textfield, printer3d_num_textarea_event_cb );

    lv_tileview_add_element( printer3d_app_setup_tile, printer3d_server_cont );
    lv_tileview_add_element( printer3d_app_setup_tile, printer3d_port_cont );
}

static void exit_printer3d_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}

static void printer3d_setup_hibernate_callback ( void ) {
    keyboard_hide();
    printer3d_setup_save_config();
}

static void printer3d_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void printer3d_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}

void printer3d_setup_save_config( void ) {
    printer3d_config_t *printer3d_config = printer3d_get_config();
    strncpy( printer3d_config->host, lv_textarea_get_text( printer3d_host_textfield ), sizeof( printer3d_config->host ) );
    printer3d_config->port = atoi(lv_textarea_get_text( printer3d_port_textfield ));
    printer3d_save_config();
}