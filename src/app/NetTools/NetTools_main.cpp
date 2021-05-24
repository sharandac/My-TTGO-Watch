/****************************************************************************
 *  NetTools_main.cpp
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

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <HTTPClient.h>

#include "NetTools.h"
#include "NetTools_main.h"  //See this file to change button text

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_styles.h"
#include "hardware/motor.h"

lv_obj_t *NetTools_main_tile = NULL;
lv_style_t NetTools_main_style;

lv_obj_t *NetTools_main_WakePC_btn = NULL;
lv_obj_t *NetTools_main_tasmota1_btn = NULL;
lv_obj_t *NetTools_main_tasmota2_btn = NULL;

lv_task_t * _NetTools_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_NetTools_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_NetTools_setup_event_cb( lv_obj_t * obj, lv_event_t event );

//Add the actions your buttons will do here.
static void wakePC_NetTools_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void tasmota1_NetTools_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void tasmota2_NetTools_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void tasmota_toggle_NetTools (int toggle_num);

void NetTools_task( lv_task_t * task );

void NetTools_main_setup( uint32_t tile_num ) {

    NetTools_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &NetTools_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( NetTools_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &NetTools_main_style );
    lv_obj_align(exit_btn, NetTools_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_NetTools_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( NetTools_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &NetTools_main_style );
    lv_obj_align(setup_btn, NetTools_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_NetTools_setup_event_cb );
    
    
    //WakePC Button
    NetTools_main_WakePC_btn = lv_btn_create(NetTools_main_tile, NULL);  
    lv_obj_set_event_cb( NetTools_main_WakePC_btn, wakePC_NetTools_main_event_cb );
    lv_obj_align(NetTools_main_WakePC_btn, NULL, LV_ALIGN_CENTER, 0, -80 );
    lv_obj_t *NetTools_main_WakePC_btn_label = lv_label_create(NetTools_main_WakePC_btn, NULL);
    lv_label_set_text(NetTools_main_WakePC_btn_label, WOL_NAME);
    
    //Tasmota1 Toggle Button
    NetTools_main_tasmota1_btn = lv_btn_create(NetTools_main_tile, NULL);  
    lv_obj_set_event_cb( NetTools_main_tasmota1_btn, tasmota1_NetTools_main_event_cb );
    lv_obj_align(NetTools_main_tasmota1_btn, NULL, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_t *NetTools_main_tasmota1_btn_label = lv_label_create(NetTools_main_tasmota1_btn, NULL);
    lv_label_set_text(NetTools_main_tasmota1_btn_label, TOGGLE1_NAME);
    
    //Tasmota2 Toggle Button
    NetTools_main_tasmota2_btn = lv_btn_create(NetTools_main_tile, NULL);  
    lv_obj_set_event_cb( NetTools_main_tasmota2_btn, tasmota2_NetTools_main_event_cb );
    lv_obj_align(NetTools_main_tasmota2_btn, NULL, LV_ALIGN_CENTER, 0, 50 );
    lv_obj_t *NetTools_main_tasmota2_btn_label = lv_label_create(NetTools_main_tasmota2_btn, NULL);
    lv_label_set_text(NetTools_main_tasmota2_btn_label, TOGGLE2_NAME);
    
    
    // create an task that runs every minute, this might be used in the future for ping status
    //    _NetTools_task = lv_task_create( NetTools_task, 60000, LV_TASK_PRIO_MID, NULL );
}

static void wakePC_NetTools_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
                                        NetTools_config_t *NetTools_config = NetTools_get_config();
                                        char *MACAddress = NetTools_config->mac_address;
                                        if (WiFi.isConnected() ){
                                            log_i("WIFI is connected, sending packet! (Target: %s)", MACAddress);
                                            WiFiUDP UDP;
                                            WakeOnLan WOL(UDP);
                                            motor_vibe(7);
                                        
                                            WOL.sendMagicPacket(MACAddress);
                                        }else{
                                            log_i("WIFI is disconnected, nothing to do.");
                                            motor_vibe(7);
                                            delay(10);
                                            motor_vibe(7);
                                        }
                                        break;
    }
}

static void tasmota1_NetTools_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      
                                        if (WiFi.isConnected()){
                                            tasmota_toggle_NetTools(1);
                                        }else{
                                            log_i("WIFI is disconnected, nothing to do.");
                                            motor_vibe(2);
                                            delay(15);
                                            motor_vibe(2);
                                        }
                                        break;
    }
}

static void tasmota2_NetTools_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      
                                        if (WiFi.isConnected()){
                                            tasmota_toggle_NetTools(2);
                                        }else{
                                            log_i("WIFI is disconnected, nothing to do.");
                                            motor_vibe(2);
                                            delay(15);
                                            motor_vibe(2);
                                        }
                                        break;
    }
}


static void tasmota_toggle_NetTools( int toggle_num){
    
    NetTools_config_t *NetTools_config = NetTools_get_config();
    
    char cmd_buffer[46] = HTTP_PREFIX;
    
    switch (toggle_num){
        case (1):
            strlcat( cmd_buffer, NetTools_config->tasmota1_ip, sizeof(cmd_buffer) );
            break;
        case (2):
            strlcat( cmd_buffer, NetTools_config->tasmota2_ip, sizeof(cmd_buffer) );
            break;
    }
    strlcat( cmd_buffer, TASMOTA_SUFFIX, sizeof(cmd_buffer) );
    log_i("Attempt to send tasmota toggle (%d) to: %s", toggle_num ,cmd_buffer);
    HTTPClient http;
    http.setTimeout(300);
    http.begin(cmd_buffer);
    http.GET();
    delay(20); //Need time for data to move through network handler routines
    http.end();
    motor_vibe(3);
}


//Enter and Exit App Events,
static void enter_NetTools_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( NetTools_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_NetTools_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

//If you need a status of something updated, such as a ping do it here...
/*void NetTools_task( lv_task_t * task ) {
    // put your code her
}*/
