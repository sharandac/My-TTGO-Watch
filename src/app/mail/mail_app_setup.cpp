/****************************************************************************
 *   Sep 29 12:17:11 2021
 *   Copyright  2021  Dirk Brosswick
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
#include "mail_app.h"
#include "mail_app_setup.h"
#include "config/mail_config.h"
#include "gui/mainbar/mainbar.h"
#include "gui/keyboard.h"
#include "gui/widget_factory.h"

lv_obj_t *mail_app_setup_tile = NULL;
lv_obj_t *mail_server_textfield = NULL;
lv_obj_t *mail_port_textfield = NULL;
lv_obj_t *mail_folder_textfield = NULL;
lv_obj_t *mail_user_textfield = NULL;
lv_obj_t *mail_password_textfield = NULL;

static void mail_setup_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void mail_setup_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void mail_setup_hibernate_callback ( void );
static void mail_setup_activate_callback ( void );

void mail_app_setup( uint32_t tile_num ) {
    
    mail_config_t *mail_config = mail_app_get_config();
    char imap_port[10];
    snprintf( imap_port, sizeof( imap_port ), "%d", mail_config->imap_port );

    mainbar_add_tile_hibernate_cb( tile_num, mail_setup_hibernate_callback );
    mainbar_add_tile_activate_cb( tile_num, mail_setup_activate_callback );

    mail_app_setup_tile = mainbar_get_tile_obj( tile_num );
    
    lv_obj_t *header = wf_add_settings_header( mail_app_setup_tile, "mail setup" );
    lv_obj_align( header, mail_app_setup_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );

    lv_obj_t *mail_server_cont = lv_obj_create( mail_app_setup_tile, NULL );
    lv_obj_set_size( mail_server_cont, lv_disp_get_hor_res( NULL ) , THEME_CONT_HEIGHT );
    lv_obj_add_style( mail_server_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( mail_server_cont, header, LV_ALIGN_OUT_BOTTOM_LEFT, -THEME_PADDING, 0 );
    lv_obj_t *mail_server_label = lv_label_create( mail_server_cont, NULL);
    lv_obj_add_style( mail_server_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( mail_server_label, "imap server");
    lv_obj_align( mail_server_label, mail_server_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    mail_server_textfield = lv_textarea_create( mail_server_cont, NULL);
    lv_textarea_set_text( mail_server_textfield, mail_config->imap_server );
    lv_textarea_set_pwd_mode( mail_server_textfield, false);
    lv_textarea_set_one_line( mail_server_textfield, true);
    lv_textarea_set_cursor_hidden( mail_server_textfield, true);
    lv_obj_set_width( mail_server_textfield, lv_disp_get_hor_res( NULL )/4 * 2 );
    lv_obj_align( mail_server_textfield, mail_server_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );
    lv_obj_set_event_cb( mail_server_textfield, mail_setup_textarea_event_cb );

    lv_obj_t *mail_port_cont = lv_obj_create( mail_app_setup_tile, NULL );
    lv_obj_set_size( mail_port_cont, lv_disp_get_hor_res( NULL ) , THEME_CONT_HEIGHT );
    lv_obj_add_style( mail_port_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( mail_port_cont, mail_server_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *mail_port_label = lv_label_create( mail_port_cont, NULL);
    lv_obj_add_style( mail_port_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( mail_port_label, "imap port");
    lv_obj_align( mail_port_label, mail_port_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    mail_port_textfield = lv_textarea_create( mail_port_cont, NULL);
    lv_textarea_set_text( mail_port_textfield, imap_port );
    lv_textarea_set_pwd_mode( mail_port_textfield, false);
    lv_textarea_set_one_line( mail_port_textfield, true);
    lv_textarea_set_cursor_hidden( mail_port_textfield, true);
    lv_obj_set_width( mail_port_textfield, lv_disp_get_hor_res( NULL )/4 * 2 );
    lv_obj_align( mail_port_textfield, mail_port_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );
    lv_obj_set_event_cb( mail_port_textfield, mail_setup_num_textarea_event_cb );

    lv_obj_t *mail_folder_cont = lv_obj_create( mail_app_setup_tile, NULL );
    lv_obj_set_size( mail_folder_cont, lv_disp_get_hor_res( NULL ) , THEME_CONT_HEIGHT );
    lv_obj_add_style( mail_folder_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( mail_folder_cont, mail_port_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *mail_folder_label = lv_label_create( mail_folder_cont, NULL);
    lv_obj_add_style( mail_folder_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( mail_folder_label, "inbox folder");
    lv_obj_align( mail_folder_label, mail_folder_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    mail_folder_textfield = lv_textarea_create( mail_folder_cont, NULL);
    lv_textarea_set_text( mail_folder_textfield, mail_config->inbox_folder );
    lv_textarea_set_pwd_mode( mail_folder_textfield, false);
    lv_textarea_set_one_line( mail_folder_textfield, true);
    lv_textarea_set_cursor_hidden( mail_folder_textfield, true);
    lv_obj_set_width( mail_folder_textfield, lv_disp_get_hor_res( NULL )/4 * 2 );
    lv_obj_align( mail_folder_textfield, mail_folder_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );
    lv_obj_set_event_cb( mail_folder_textfield, mail_setup_textarea_event_cb );

    lv_obj_t *mail_user_cont = lv_obj_create( mail_app_setup_tile, NULL );
    lv_obj_set_size( mail_user_cont, lv_disp_get_hor_res( NULL ) , THEME_CONT_HEIGHT );
    lv_obj_add_style( mail_user_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( mail_user_cont, mail_folder_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *mail_user_label = lv_label_create( mail_user_cont, NULL);
    lv_obj_add_style( mail_user_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( mail_user_label, "user name");
    lv_obj_align( mail_user_label, mail_user_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    mail_user_textfield = lv_textarea_create( mail_user_cont, NULL);
    lv_textarea_set_text( mail_user_textfield, mail_config->username );
    lv_textarea_set_pwd_mode( mail_user_textfield, false);
    lv_textarea_set_one_line( mail_user_textfield, true);
    lv_textarea_set_cursor_hidden( mail_user_textfield, true);
    lv_obj_set_width( mail_user_textfield, lv_disp_get_hor_res( NULL )/4 * 2 );
    lv_obj_align( mail_user_textfield, mail_user_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );
    lv_obj_set_event_cb( mail_user_textfield, mail_setup_textarea_event_cb );

    lv_obj_t *mail_password_cont = lv_obj_create( mail_app_setup_tile, NULL );
    lv_obj_set_size( mail_password_cont, lv_disp_get_hor_res( NULL ) , THEME_CONT_HEIGHT );
    lv_obj_add_style( mail_password_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( mail_password_cont, mail_user_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *mail_password_label = lv_label_create( mail_password_cont, NULL);
    lv_obj_add_style( mail_password_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( mail_password_label, "imap server");
    lv_obj_align( mail_password_label, mail_password_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    mail_password_textfield = lv_textarea_create( mail_password_cont, NULL);
    lv_textarea_set_text( mail_password_textfield,mail_config->password );
    lv_textarea_set_pwd_mode( mail_password_textfield, false);
    lv_textarea_set_one_line( mail_password_textfield, true);
    lv_textarea_set_cursor_hidden( mail_password_textfield, true);
    lv_obj_set_width( mail_password_textfield, lv_disp_get_hor_res( NULL )/4 * 2 );
    lv_obj_align( mail_password_textfield, mail_password_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );
    lv_obj_set_event_cb( mail_password_textfield, mail_setup_textarea_event_cb );
}

static void mail_setup_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void mail_setup_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}

static void mail_setup_hibernate_callback ( void ) {
    /**
     * hide keyboard if we exit setup
     */
    keyboard_hide();
    /**
     * get mail config
     */
    mail_config_t *mail_config = mail_app_get_config();
    /**
     * copy all config data into mail config
     */
    strncpy( mail_config->imap_server, lv_textarea_get_text( mail_server_textfield ), sizeof( mail_config->imap_server ) );
    mail_config->imap_port = atoi( lv_textarea_get_text( mail_port_textfield ) );
    strncpy( mail_config->username, lv_textarea_get_text( mail_user_textfield ), sizeof( mail_config->username ) );
    strncpy( mail_config->password, lv_textarea_get_text( mail_password_textfield ), sizeof( mail_config->password ) );
    /**
     * store mail config
     */
    mail_config->save();
}

static void mail_setup_activate_callback ( void ) {
    /**
     * get mail config
     */
    mail_config_t *mail_config = mail_app_get_config();
    /**
     * reload mail config
     */
    mail_config->load();

    char imap_port[10];
    snprintf( imap_port, sizeof( imap_port ), "%d", mail_config->imap_port );
    /**
     * copy all config data into mail config
     */
    lv_textarea_set_text( mail_server_textfield, mail_config->imap_server );
    lv_textarea_set_text( mail_port_textfield, imap_port );
    lv_textarea_set_text( mail_user_textfield, mail_config->username );
    lv_textarea_set_text( mail_password_textfield, mail_config->password );
}