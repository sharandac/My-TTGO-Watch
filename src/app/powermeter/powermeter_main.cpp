/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#include <TTGO.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "powermeter_app.h"
#include "powermeter_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#include "hardware/wifictl.h"
#include "hardware/json_psram_allocator.h"
#include "hardware/alloc.h"

lv_obj_t *powermeter_main_tile = NULL;
lv_style_t powermeter_main_style;
lv_style_t powermeter_id_style;

lv_task_t * _powermeter_main_task;

lv_obj_t *id_cont = NULL;
lv_obj_t *id_label = NULL;
lv_obj_t *voltage_cont = NULL;
lv_obj_t *voltage_label = NULL;
lv_obj_t *current_cont = NULL;
lv_obj_t *current_label = NULL;
lv_obj_t *power_cont = NULL;
lv_obj_t *power_label = NULL;

WiFiClient espClient;
PubSubClient powermeter_mqtt_client( espClient );

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_48px);

bool powermeter_wifictl_event_cb( EventBits_t event, void *arg );
static void exit_powermeter_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_powermeter_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void powermeter_main_task( lv_task_t * task );

void callback(char* topic, byte* payload, unsigned int length) {
    char *msg = NULL;
    msg = (char*)CALLOC( length + 1, 1 );
    if ( msg == NULL ) {
        log_e("calloc failed");
        return;
    }
    memcpy( msg, payload, length );

    SpiRamJsonDocument doc( strlen( msg ) * 2 );

    DeserializationError error = deserializeJson( doc, msg );
    if ( error ) {
        log_e("powermeter message deserializeJson() failed: %s", error.c_str() );
    }
    else  {
        if ( doc["id"] ) {
            lv_label_set_text( id_label, doc["id"] );
        }
        if ( doc["all"]["power"] ) {
            char temp[16] = "";
            snprintf( temp, sizeof( temp ), "%0.2fkW", atof( doc["all"]["power"] ) );
            widget_set_label( powermeter_get_widget_icon(), temp );
        }
        if ( doc["channel0"]["power"] ) {
            char temp[16] = "";
            snprintf( temp, sizeof( temp ), "%0.2fkW", atof( doc["channel0"]["power"] ) );
            lv_label_set_text( power_label, temp );
        }
        if ( doc["channel0"]["voltage"] ) {
            char temp[16] = "";
            snprintf( temp, sizeof( temp ), "%0.1fV", atof( doc["channel0"]["voltage"] ) );
            lv_label_set_text( voltage_label, temp );
        }
        if ( doc["channel0"]["current"] ) {
            char temp[16] = "";
            snprintf( temp, sizeof( temp ), "%0.1fA", atof( doc["channel0"]["current"] ) );
            lv_label_set_text( current_label, temp );
        }
        lv_obj_align( id_label, id_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
        lv_obj_align( power_label, power_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
        lv_obj_align( voltage_label, voltage_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
        lv_obj_align( current_label, current_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    }
    doc.clear();
    free( msg );
}

void powermeter_main_tile_setup( uint32_t tile_num ) {

    powermeter_main_tile = mainbar_get_tile_obj( tile_num );

    lv_style_copy( &powermeter_main_style, mainbar_get_style() );
    lv_style_set_bg_color( &powermeter_main_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &powermeter_main_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &powermeter_main_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &powermeter_main_style, LV_STATE_DEFAULT, &Ubuntu_48px);
    lv_obj_add_style( powermeter_main_tile, LV_OBJ_PART_MAIN, &powermeter_main_style );

    lv_style_copy( &powermeter_id_style, mainbar_get_style() );
    lv_style_set_bg_color( &powermeter_id_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &powermeter_id_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &powermeter_id_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &powermeter_id_style, LV_STATE_DEFAULT, &Ubuntu_16px);

    lv_obj_t * exit_btn = lv_imgbtn_create( powermeter_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &powermeter_main_style );
    lv_obj_align(exit_btn, powermeter_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_powermeter_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( powermeter_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &powermeter_main_style );
    lv_obj_align(setup_btn, powermeter_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_powermeter_setup_event_cb );

    id_cont = lv_obj_create( powermeter_main_tile, NULL );
    lv_obj_set_size( id_cont, lv_disp_get_hor_res( NULL ), 20 );
    lv_obj_add_style( id_cont, LV_OBJ_PART_MAIN, &powermeter_id_style );
    lv_obj_align( id_cont, powermeter_main_tile, LV_ALIGN_IN_TOP_MID, 0, 0 );
    lv_obj_t * id_info_label = lv_label_create( id_cont, NULL );
    lv_obj_add_style( id_info_label, LV_OBJ_PART_MAIN, &powermeter_id_style );
    lv_label_set_text( id_info_label, "ID:" );
    lv_obj_align( id_info_label, id_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    id_label = lv_label_create( id_cont, NULL );
    lv_obj_add_style( id_label, LV_OBJ_PART_MAIN, &powermeter_id_style );
    lv_label_set_text( id_label, "n/a" );
    lv_obj_align( id_label, id_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    voltage_cont = lv_obj_create( powermeter_main_tile, NULL );
    lv_obj_set_size( voltage_cont, lv_disp_get_hor_res( NULL ), 56 );
    lv_obj_add_style( voltage_cont, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_obj_align( voltage_cont, id_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t * voltage_info_label = lv_label_create( voltage_cont, NULL );
    lv_obj_add_style( voltage_info_label, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_label_set_text( voltage_info_label, "U =" );
    lv_obj_align( voltage_info_label, voltage_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    voltage_label = lv_label_create( voltage_cont, NULL );
    lv_obj_add_style( voltage_label, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_label_set_text( voltage_label, "n/a" );
    lv_obj_align( voltage_label, voltage_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    current_cont = lv_obj_create( powermeter_main_tile, NULL );
    lv_obj_set_size( current_cont, lv_disp_get_hor_res( NULL ), 56 );
    lv_obj_add_style( current_cont, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_obj_align( current_cont, voltage_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t * current_info_label = lv_label_create( current_cont, NULL );
    lv_obj_add_style( current_info_label, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_label_set_text( current_info_label, "I =" );
    lv_obj_align( current_info_label, current_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    current_label = lv_label_create( current_cont, NULL );
    lv_obj_add_style( current_label, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_label_set_text( current_label, "n/a" );
    lv_obj_align( current_label, current_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    power_cont = lv_obj_create( powermeter_main_tile, NULL );
    lv_obj_set_size( power_cont, lv_disp_get_hor_res( NULL ), 56 );
    lv_obj_add_style( power_cont, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_obj_align( power_cont, current_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t * power_info_label = lv_label_create( power_cont, NULL );
    lv_obj_add_style( power_info_label, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_label_set_text( power_info_label, "P =" );
    lv_obj_align( power_info_label, power_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    power_label = lv_label_create( power_cont, NULL );
    lv_obj_add_style( power_label, LV_OBJ_PART_MAIN, &powermeter_main_style );
    lv_label_set_text( power_label, "n/a" );
    lv_obj_align( power_label, power_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    powermeter_mqtt_client.setCallback( callback );
    powermeter_mqtt_client.setBufferSize( 512 );

    wifictl_register_cb( WIFICTL_CONNECT_IP | WIFICTL_OFF_REQUEST | WIFICTL_OFF | WIFICTL_DISCONNECT , powermeter_wifictl_event_cb, "powermeter" );
    // create an task that runs every secound
    _powermeter_main_task = lv_task_create( powermeter_main_task, 250, LV_TASK_PRIO_MID, NULL );
}

bool powermeter_wifictl_event_cb( EventBits_t event, void *arg ) {
    powermeter_config_t *powermeter_config = powermeter_get_config();
    switch( event ) {
        case WIFICTL_CONNECT_IP:    if ( powermeter_config->autoconnect ) {
                                        powermeter_mqtt_client.setServer( powermeter_config->server, powermeter_config->port );
                                        if ( !powermeter_mqtt_client.connect( "powermeter", powermeter_config->user, powermeter_config->password ) ) {
                                            log_e("connect to mqtt server %s failed", powermeter_config->server );
                                            app_set_indicator( powermeter_get_app_icon(), ICON_INDICATOR_FAIL );
                                            widget_set_indicator( powermeter_get_widget_icon() , ICON_INDICATOR_FAIL );
                                        }
                                        else {
                                            log_i("connect to mqtt server %s success", powermeter_config->server );
                                            powermeter_mqtt_client.subscribe( powermeter_config->topic );
                                            app_set_indicator( powermeter_get_app_icon(), ICON_INDICATOR_OK );
                                            widget_set_indicator( powermeter_get_widget_icon(), ICON_INDICATOR_OK );
                                        }
                                    } 
                                    break;
        case WIFICTL_OFF_REQUEST:
        case WIFICTL_OFF:
        case WIFICTL_DISCONNECT:    log_i("disconnect from mqtt server %s", powermeter_config->server );
                                    powermeter_mqtt_client.disconnect();
                                    app_hide_indicator( powermeter_get_app_icon() );
                                    widget_hide_indicator( powermeter_get_widget_icon() );
                                    widget_set_label( powermeter_get_widget_icon(), "n/a" );
                                    break;
    }
    return( true );
}

static void enter_powermeter_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( powermeter_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_powermeter_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void powermeter_main_task( lv_task_t * task ) {
    // put your code her
    powermeter_mqtt_client.loop();
}