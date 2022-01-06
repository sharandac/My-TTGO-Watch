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
#include <config.h>

#include "powermeter_app.h"
#include "powermeter_setup.h"
#include "config/powermeter_config.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
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
#endif

lv_obj_t *powermeter_setup_tile = NULL;
lv_obj_t *powermeter_setup_tile_2 = NULL;
uint32_t powermeter_setup_tile_num;

lv_obj_t *powermeter_server_textfield = NULL;
lv_obj_t *powermeter_user_textfield = NULL;
lv_obj_t *powermeter_password_textfield = NULL;
lv_obj_t *powermeter_topic_textfield = NULL;
lv_obj_t *powermeter_port_textfield = NULL;
lv_obj_t *powermeter_autoconnect_onoff = NULL;
lv_obj_t *powermeter_widget_onoff = NULL;

static void powermeter_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void powermeter_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void powermeter_autoconnect_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
static void powermeter_widget_onoff_event_handler( lv_obj_t *obj, lv_event_t event );
static void powermeter_setup_page_2_event_cb( lv_obj_t *obj, lv_event_t event );
static void powermeter_setup_hibernate_callback ( void );

void powermeter_setup_tile_setup( uint32_t tile_num ) {

    powermeter_config_t *powermeter_config = powermeter_get_config();

    mainbar_add_tile_hibernate_cb( tile_num, powermeter_setup_hibernate_callback );

    powermeter_setup_tile_num = tile_num;
    powermeter_setup_tile = mainbar_get_tile_obj( powermeter_setup_tile_num );
    powermeter_setup_tile_2 = mainbar_get_tile_obj( powermeter_setup_tile_num + 1 );

    lv_obj_t *header = wf_add_settings_header( powermeter_setup_tile, "powermeter setup" );
    lv_obj_align( header, powermeter_setup_tile, LV_ALIGN_IN_TOP_LEFT, THEME_ICON_PADDING, THEME_ICON_PADDING );

    lv_obj_t *header_2 = wf_add_settings_header( powermeter_setup_tile_2, "powermeter setup" );
    lv_obj_align( header_2, powermeter_setup_tile_2, LV_ALIGN_IN_TOP_LEFT, THEME_ICON_PADDING, THEME_ICON_PADDING );

    lv_obj_t *setup_page_2 = wf_add_down_button( powermeter_setup_tile, powermeter_setup_page_2_event_cb );
    lv_obj_align( setup_page_2, powermeter_setup_tile, LV_ALIGN_IN_TOP_RIGHT, -THEME_ICON_PADDING, THEME_ICON_PADDING );

    lv_obj_t *powermeter_server_cont = lv_obj_create( powermeter_setup_tile, NULL );
    lv_obj_set_size( powermeter_server_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( powermeter_server_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( powermeter_server_cont, header, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );
    lv_obj_t *powermeter_server_label = lv_label_create( powermeter_server_cont, NULL);
    lv_obj_add_style( powermeter_server_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( powermeter_server_label, "server");
    lv_obj_align( powermeter_server_label, powermeter_server_cont, LV_ALIGN_IN_LEFT_MID, 0, 0 );
    powermeter_server_textfield = lv_textarea_create( powermeter_server_cont, NULL);
    lv_textarea_set_text( powermeter_server_textfield, powermeter_config->server );
    lv_textarea_set_pwd_mode( powermeter_server_textfield, false);
    lv_textarea_set_one_line( powermeter_server_textfield, true);
    lv_textarea_set_cursor_hidden( powermeter_server_textfield, true);
    lv_obj_set_width( powermeter_server_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - 5 );
    lv_obj_align( powermeter_server_textfield, powermeter_server_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( powermeter_server_textfield, powermeter_textarea_event_cb );

    lv_obj_t *powermeter_port_cont = lv_obj_create( powermeter_setup_tile, NULL );
    lv_obj_set_size( powermeter_port_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( powermeter_port_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( powermeter_port_cont, powermeter_server_cont, LV_ALIGN_OUT_BOTTOM_MID, 0,  0 );
    lv_obj_t *powermeter_port_label = lv_label_create( powermeter_port_cont, NULL);
    lv_obj_add_style( powermeter_port_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( powermeter_port_label, "port");
    lv_obj_align( powermeter_port_label, powermeter_port_cont, LV_ALIGN_IN_LEFT_MID, 0, 0 );
    powermeter_port_textfield = lv_textarea_create( powermeter_port_cont, NULL);
    char buf[10];
    sprintf(buf, "%d", powermeter_config->port );
    lv_textarea_set_text( powermeter_port_textfield, buf);
    lv_textarea_set_pwd_mode( powermeter_port_textfield, false);
    lv_textarea_set_one_line( powermeter_port_textfield, true);
    lv_textarea_set_cursor_hidden( powermeter_port_textfield, true);
    lv_obj_set_width( powermeter_port_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - 5 );
    lv_obj_align( powermeter_port_textfield, powermeter_port_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( powermeter_port_textfield, powermeter_num_textarea_event_cb );

    lv_obj_t *powermeter_user_cont = lv_obj_create( powermeter_setup_tile, NULL );
    lv_obj_set_size( powermeter_user_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( powermeter_user_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( powermeter_user_cont, powermeter_port_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *powermeter_user_label = lv_label_create( powermeter_user_cont, NULL);
    lv_obj_add_style( powermeter_user_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( powermeter_user_label, "user");
    lv_obj_align( powermeter_user_label, powermeter_user_cont, LV_ALIGN_IN_LEFT_MID, 0, 0 );
    powermeter_user_textfield = lv_textarea_create( powermeter_user_cont, NULL);
    lv_textarea_set_text( powermeter_user_textfield, powermeter_config->user );
    lv_textarea_set_pwd_mode( powermeter_user_textfield, false);
    lv_textarea_set_one_line( powermeter_user_textfield, true);
    lv_textarea_set_cursor_hidden( powermeter_user_textfield, true);
    lv_obj_set_width( powermeter_user_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - 5 );
    lv_obj_align( powermeter_user_textfield, powermeter_user_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( powermeter_user_textfield, powermeter_textarea_event_cb );

    lv_obj_t *powermeter_password_cont = lv_obj_create( powermeter_setup_tile, NULL );
    lv_obj_set_size( powermeter_password_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( powermeter_password_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( powermeter_password_cont, powermeter_user_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *powermeter_password_label = lv_label_create( powermeter_password_cont, NULL);
    lv_obj_add_style( powermeter_password_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( powermeter_password_label, "pass");
    lv_obj_align( powermeter_password_label, powermeter_password_cont, LV_ALIGN_IN_LEFT_MID, 0, 0 );
    powermeter_password_textfield = lv_textarea_create( powermeter_password_cont, NULL);
    lv_textarea_set_text( powermeter_password_textfield, powermeter_config->password );
    lv_textarea_set_pwd_mode( powermeter_password_textfield, false);
    lv_textarea_set_one_line( powermeter_password_textfield, true);
    lv_textarea_set_cursor_hidden( powermeter_password_textfield, true);
    lv_obj_set_width( powermeter_password_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - 5 );
    lv_obj_align( powermeter_password_textfield, powermeter_password_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( powermeter_password_textfield, powermeter_textarea_event_cb );

    lv_obj_t *powermeter_topic_cont = lv_obj_create( powermeter_setup_tile, NULL );
    lv_obj_set_size( powermeter_topic_cont, lv_disp_get_hor_res( NULL ) , 37);
    lv_obj_add_style( powermeter_topic_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( powermeter_topic_cont, powermeter_password_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *powermeter_topic_label = lv_label_create( powermeter_topic_cont, NULL);
    lv_obj_add_style( powermeter_topic_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( powermeter_topic_label, "topic");
    lv_obj_align( powermeter_topic_label, powermeter_topic_cont, LV_ALIGN_IN_LEFT_MID, 0, 0 );
    powermeter_topic_textfield = lv_textarea_create( powermeter_topic_cont, NULL);
    lv_textarea_set_text( powermeter_topic_textfield, powermeter_config->topic );
    lv_textarea_set_pwd_mode( powermeter_topic_textfield, false);
    lv_textarea_set_one_line( powermeter_topic_textfield, true);
    lv_textarea_set_cursor_hidden( powermeter_topic_textfield, true);
    lv_obj_set_width( powermeter_topic_textfield, lv_disp_get_hor_res( NULL ) / 4 * 3  - 5 );
    lv_obj_align( powermeter_topic_textfield, powermeter_topic_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( powermeter_topic_textfield, powermeter_textarea_event_cb );

    lv_obj_t *powermeter_autoconnect_onoff_cont = lv_obj_create( powermeter_setup_tile_2, NULL);
    lv_obj_set_size( powermeter_autoconnect_onoff_cont, lv_disp_get_hor_res( NULL ), 32);
    lv_obj_add_style( powermeter_autoconnect_onoff_cont, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_obj_align( powermeter_autoconnect_onoff_cont, header_2, LV_ALIGN_OUT_BOTTOM_LEFT, -THEME_ICON_PADDING, THEME_ICON_PADDING );
    powermeter_autoconnect_onoff = wf_add_switch( powermeter_autoconnect_onoff_cont, false);
    lv_obj_align( powermeter_autoconnect_onoff, powermeter_autoconnect_onoff_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_set_event_cb( powermeter_autoconnect_onoff, powermeter_autoconnect_onoff_event_handler );
    lv_obj_t *powermeter_autoconnect_label = lv_label_create(powermeter_autoconnect_onoff_cont, NULL);
    lv_obj_add_style( powermeter_autoconnect_label, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_label_set_text( powermeter_autoconnect_label, "autoconnect");
    lv_obj_align( powermeter_autoconnect_label, powermeter_autoconnect_onoff_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_t *powermeter_widget_onoff_cont = lv_obj_create( powermeter_setup_tile_2, NULL);
    lv_obj_set_size( powermeter_widget_onoff_cont, lv_disp_get_hor_res( NULL ), 32);
    lv_obj_add_style( powermeter_widget_onoff_cont, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_obj_align( powermeter_widget_onoff_cont, powermeter_autoconnect_onoff_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );
    powermeter_widget_onoff = wf_add_switch( powermeter_widget_onoff_cont, false);
    lv_obj_align( powermeter_widget_onoff, powermeter_widget_onoff_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_set_event_cb( powermeter_widget_onoff, powermeter_widget_onoff_event_handler );
    lv_obj_t *powermeter_widget_onoff_label = lv_label_create( powermeter_widget_onoff_cont, NULL);
    lv_obj_add_style( powermeter_widget_onoff_label, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_label_set_text( powermeter_widget_onoff_label, "mainbar widget");
    lv_obj_align( powermeter_widget_onoff_label, powermeter_widget_onoff_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);

    if ( powermeter_config->autoconnect )
        lv_switch_on( powermeter_autoconnect_onoff, LV_ANIM_OFF);
    else
        lv_switch_off( powermeter_autoconnect_onoff, LV_ANIM_OFF);

    if ( powermeter_config->widget )
        lv_switch_on( powermeter_widget_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( powermeter_widget_onoff, LV_ANIM_OFF );

    lv_tileview_add_element( powermeter_setup_tile, powermeter_server_cont );
    lv_tileview_add_element( powermeter_setup_tile, powermeter_port_cont );
    lv_tileview_add_element( powermeter_setup_tile, powermeter_user_cont );
    lv_tileview_add_element( powermeter_setup_tile, powermeter_password_cont );
    lv_tileview_add_element( powermeter_setup_tile, powermeter_topic_cont );
}

static void powermeter_setup_hibernate_callback ( void ) {
    keyboard_hide();
    powermeter_config_t *powermeter_config = powermeter_get_config();
    strncpy( powermeter_config->server, lv_textarea_get_text( powermeter_server_textfield ), sizeof( powermeter_config->server ) );
    strncpy( powermeter_config->user, lv_textarea_get_text( powermeter_user_textfield ), sizeof( powermeter_config->user ) );
    strncpy( powermeter_config->password, lv_textarea_get_text( powermeter_password_textfield ), sizeof( powermeter_config->password ) );
    strncpy( powermeter_config->topic, lv_textarea_get_text( powermeter_topic_textfield ), sizeof( powermeter_config->topic ) );
    powermeter_config->port = atoi(lv_textarea_get_text( powermeter_port_textfield ));
    powermeter_config->save();
}

static void powermeter_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void powermeter_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}

static void powermeter_setup_page_2_event_cb( lv_obj_t *obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        mainbar_jump_to_tilenumber( powermeter_setup_tile_num + 1, LV_ANIM_OFF );
    }
    
}
static void powermeter_autoconnect_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case (LV_EVENT_VALUE_CHANGED):      powermeter_config_t *powermeter_config = powermeter_get_config();
                                            powermeter_config->autoconnect = lv_switch_get_state( obj );
                                            break;
    }
}

static void powermeter_widget_onoff_event_handler(lv_obj_t *obj, lv_event_t event)
{
    switch ( event ) {
        case ( LV_EVENT_VALUE_CHANGED ):    powermeter_config_t *powermeter_config = powermeter_get_config();
                                            powermeter_config->widget = lv_switch_get_state( obj );
                                            if ( powermeter_config->widget ) {
                                                powermeter_add_widget();
                                            }
                                            else {
                                                powermeter_remove_widget();
                                            }
                                            break;
    }
}
