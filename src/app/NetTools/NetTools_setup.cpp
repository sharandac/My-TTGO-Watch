/****************************************************************************
 *  NetTools_setup.cpp
 *  Copyright  2020  David Stewart / NorthernDIY
 *  Email: genericsoftwaredeveloper@gmail.com
 *
 *  Requires Libraries: 
 *      WakeOnLan by a7md0      https://github.com/a7md0/WakeOnLan
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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
#include <TTGO.h>

#include "NetTools.h"
#include "NetTools_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_styles.h"
#define INPUT_MAC_LENGTH 20
#define INPUT_IP_LENGTH 18

lv_obj_t *NetTools_setup_tile = NULL;
lv_style_t NetTools_setup_style;

lv_obj_t *NetTools_foobar_switch = NULL;
lv_obj_t *NetTools_WOL_MAC_textfield = NULL;
lv_obj_t *NetTools_Tasmota1_IP_textfield = NULL;
lv_obj_t *NetTools_Tasmota2_IP_textfield = NULL;
lv_obj_t *NetTools_Tasmota3_IP_textfield = NULL;


LV_IMG_DECLARE(exit_32px);

static void exit_NetTools_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void NetTools_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event );
static void NetTools_textarea_event_cb( lv_obj_t * obj, lv_event_t event );

void NetTools_setup_setup( uint32_t tile_num ) {
    
    NetTools_config_t *NetTools_config = NetTools_get_config();

    NetTools_setup_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &NetTools_setup_style, ws_get_mainbar_style() );

    lv_style_set_bg_color( &NetTools_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &NetTools_setup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &NetTools_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( NetTools_setup_tile, LV_OBJ_PART_MAIN, &NetTools_setup_style );

    lv_obj_t *exit_cont = lv_obj_create( NetTools_setup_tile, NULL );
    lv_obj_set_size( exit_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( exit_cont, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_obj_align( exit_cont, NetTools_setup_tile, LV_ALIGN_IN_TOP_MID, 0, 10 );

    lv_obj_t *exit_btn = lv_imgbtn_create( exit_cont, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &NetTools_setup_style );
    lv_obj_align( exit_btn, exit_cont, LV_ALIGN_IN_TOP_LEFT, 10, 0 );
    lv_obj_set_event_cb( exit_btn, exit_NetTools_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( exit_cont, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_label_set_text( exit_label, "NetTools Setup");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );
    
    /*
    lv_obj_t *NetTools_foobar_switch_cont = lv_obj_create( NetTools_setup_tile, NULL );
    lv_obj_set_size( NetTools_foobar_switch_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( NetTools_foobar_switch_cont, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_obj_align( NetTools_foobar_switch_cont, exit_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );

    NetTools_foobar_switch = lv_switch_create( NetTools_foobar_switch_cont, NULL );
    lv_obj_add_protect( NetTools_foobar_switch, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( NetTools_foobar_switch, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( NetTools_foobar_switch, LV_ANIM_ON );
    lv_obj_align( NetTools_foobar_switch, NetTools_foobar_switch_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( NetTools_foobar_switch, NetTools_foobar_switch_event_cb );

    lv_obj_t *NetTools_foobar_switch_label = lv_label_create( NetTools_foobar_switch_cont, NULL);
    lv_obj_add_style( NetTools_foobar_switch_label, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_label_set_text( NetTools_foobar_switch_label, "Foo Bar");
    lv_obj_align( NetTools_foobar_switch_label, NetTools_foobar_switch_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    */
    //WakePC Mac Address
    lv_obj_t *NetTools_WOL_MAC_cont = lv_obj_create( NetTools_setup_tile, NULL );
    lv_obj_set_size(NetTools_WOL_MAC_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( NetTools_WOL_MAC_cont, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_obj_align( NetTools_WOL_MAC_cont, exit_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20 );
    lv_obj_t *NetTools_WOL_MAC_label = lv_label_create( NetTools_WOL_MAC_cont, NULL);
    lv_obj_add_style( NetTools_WOL_MAC_label, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_label_set_text( NetTools_WOL_MAC_label, "WOL MAC:");
    lv_obj_align( NetTools_WOL_MAC_label, NetTools_WOL_MAC_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    NetTools_WOL_MAC_textfield = lv_textarea_create( NetTools_WOL_MAC_cont, NULL);
    lv_textarea_set_text( NetTools_WOL_MAC_textfield, NetTools_config->mac_address );
    lv_textarea_set_pwd_mode( NetTools_WOL_MAC_textfield, false);
    lv_textarea_set_one_line( NetTools_WOL_MAC_textfield, true);
    lv_textarea_set_cursor_hidden( NetTools_WOL_MAC_textfield, true);
    lv_obj_set_width( NetTools_WOL_MAC_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( NetTools_WOL_MAC_textfield, NetTools_WOL_MAC_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( NetTools_WOL_MAC_textfield, NetTools_textarea_event_cb );
    lv_textarea_set_accepted_chars(NetTools_WOL_MAC_textfield, "0123456789ABCDEF:");
    lv_textarea_set_max_length(NetTools_WOL_MAC_textfield, INPUT_MAC_LENGTH);
    
    //Tasmota 1 Ip Address
    lv_obj_t *NetTools_Tasmota1_IP_cont = lv_obj_create( NetTools_setup_tile, NULL );
    lv_obj_set_size(NetTools_Tasmota1_IP_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( NetTools_Tasmota1_IP_cont, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_obj_align( NetTools_Tasmota1_IP_cont, NetTools_WOL_MAC_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10 );
    lv_obj_t *NetTools_Tasmota1_IP_label = lv_label_create( NetTools_Tasmota1_IP_cont, NULL);
    lv_obj_add_style( NetTools_Tasmota1_IP_label, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_label_set_text( NetTools_Tasmota1_IP_label, "Tasmota 1 IP:");
    lv_obj_align( NetTools_Tasmota1_IP_label, NetTools_Tasmota1_IP_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    NetTools_Tasmota1_IP_textfield = lv_textarea_create( NetTools_Tasmota1_IP_cont, NULL);
    lv_textarea_set_text( NetTools_Tasmota1_IP_textfield, NetTools_config->tasmota1_ip );
    lv_textarea_set_pwd_mode( NetTools_Tasmota1_IP_textfield, false);
    lv_textarea_set_one_line( NetTools_Tasmota1_IP_textfield, true);
    lv_textarea_set_cursor_hidden( NetTools_Tasmota1_IP_textfield, true);
    lv_obj_set_width( NetTools_Tasmota1_IP_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( NetTools_Tasmota1_IP_textfield, NetTools_Tasmota1_IP_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( NetTools_Tasmota1_IP_textfield, NetTools_textarea_event_cb );
    lv_textarea_set_accepted_chars(NetTools_Tasmota1_IP_textfield, "0123456789.");
    lv_textarea_set_max_length(NetTools_Tasmota1_IP_textfield, INPUT_IP_LENGTH);
    
    //Tasmota 2 Ip Address
    lv_obj_t *NetTools_Tasmota2_IP_cont = lv_obj_create( NetTools_setup_tile, NULL );
    lv_obj_set_size(NetTools_Tasmota2_IP_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( NetTools_Tasmota2_IP_cont, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_obj_align( NetTools_Tasmota2_IP_cont, NetTools_Tasmota1_IP_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10 );
    lv_obj_t *NetTools_Tasmota2_IP_label = lv_label_create( NetTools_Tasmota2_IP_cont, NULL);
    lv_obj_add_style( NetTools_Tasmota2_IP_label, LV_OBJ_PART_MAIN, &NetTools_setup_style  );
    lv_label_set_text( NetTools_Tasmota2_IP_label, "Tasmota 2 IP:");
    lv_obj_align( NetTools_Tasmota2_IP_label, NetTools_Tasmota2_IP_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    NetTools_Tasmota2_IP_textfield = lv_textarea_create( NetTools_Tasmota2_IP_cont, NULL);
    lv_textarea_set_text( NetTools_Tasmota2_IP_textfield, NetTools_config->tasmota2_ip );
    lv_textarea_set_pwd_mode( NetTools_Tasmota2_IP_textfield, false);
    lv_textarea_set_one_line( NetTools_Tasmota2_IP_textfield, true);
    lv_textarea_set_cursor_hidden( NetTools_Tasmota2_IP_textfield, true);
    lv_obj_set_width( NetTools_Tasmota2_IP_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( NetTools_Tasmota2_IP_textfield, NetTools_Tasmota2_IP_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( NetTools_Tasmota2_IP_textfield, NetTools_textarea_event_cb );
    lv_textarea_set_accepted_chars(NetTools_Tasmota2_IP_textfield, "0123456789.");
    lv_textarea_set_max_length(NetTools_Tasmota2_IP_textfield, INPUT_IP_LENGTH);
    
}

static void NetTools_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): Serial.printf( "switch value = %d\r\n", lv_switch_get_state( obj ) );
                                        break;
    }
}

static void NetTools_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void exit_NetTools_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        keyboard_hide();
                                        NetTools_config_t *NetTools_config = NetTools_get_config();
                                        //WOL Entry
                                        strlcpy( NetTools_config->mac_address, lv_textarea_get_text( NetTools_WOL_MAC_textfield ), sizeof( NetTools_config->mac_address ) );

                                        //Tasmota Entries
                                        strlcpy( NetTools_config->tasmota1_ip, lv_textarea_get_text( NetTools_Tasmota1_IP_textfield ), sizeof( NetTools_config->tasmota1_ip ) );
                                        strlcpy( NetTools_config->tasmota2_ip, lv_textarea_get_text( NetTools_Tasmota2_IP_textfield ), sizeof( NetTools_config->tasmota2_ip ) );
                                        
                                        
                                        NetTools_save_config();
                                        mainbar_jump_to_tilenumber( NetTools_get_app_main_tile_num(), LV_ANIM_ON );
                                        break;
    }
}
