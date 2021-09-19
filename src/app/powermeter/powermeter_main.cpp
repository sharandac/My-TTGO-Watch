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

#include "powermeter_app.h"
#include "powermeter_main.h"
#include "config/powermeter_config.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/wifictl.h"

#include "utils/json_psram_allocator.h"
#include "utils/alloc.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>
    #include <mosquitto.h>

    using namespace std;
    #define String string

    struct mosquitto *mosq;
#else
    #include <Arduino.h>
    #include <WiFi.h>
    #include <PubSubClient.h>

    WiFiClient espClient;
    PubSubClient powermeter_mqtt_client( espClient );
#endif

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

LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_48px);

bool powermeter_style_change_event_cb( EventBits_t event, void *arg );
bool powermeter_wifictl_event_cb( EventBits_t event, void *arg );
static void enter_powermeter_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void powermeter_main_task( lv_task_t * task );

#ifdef NATIVE_64BIT
    void on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
        powermeter_config_t *powermeter_config = powermeter_get_config();
        int rc;
        log_i("on_connect: %s\n", mosquitto_connack_string( reason_code ) );
        if(reason_code != 0){
            mosquitto_disconnect(mosq);
        }
        rc = mosquitto_subscribe(mosq, NULL, powermeter_config->topic, 1);
        if(rc != MOSQ_ERR_SUCCESS){
            log_i( "Error subscribing: %s\n", mosquitto_strerror( rc ) );
            mosquitto_disconnect(mosq);
        }
    }

    void callback (struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)  {
        unsigned int length = msg->payloadlen;
        char *payload = (char *)msg->payload;
#else
    void callback(char* topic, byte* payload, unsigned int length) {
#endif
    /**
     * alloc a msg buffer and copy payload and terminate it with '\0';
     */
    char *mqttmsg = NULL;
    mqttmsg = (char*)CALLOC( length + 1, 1 );
    if ( mqttmsg == NULL ) {
        log_e("calloc failed");
        return;
    }
    memcpy( mqttmsg, payload, length );

    SpiRamJsonDocument doc( strlen( mqttmsg ) * 2 );
    DeserializationError error = deserializeJson( doc, mqttmsg );

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
    free( mqttmsg );
}

void powermeter_main_tile_setup( uint32_t tile_num ) {

    powermeter_main_tile = mainbar_get_tile_obj( tile_num );

    lv_style_copy( &powermeter_main_style, APP_STYLE );
    lv_style_set_text_font( &powermeter_main_style, LV_STATE_DEFAULT, &Ubuntu_48px);
    lv_obj_add_style( powermeter_main_tile, LV_OBJ_PART_MAIN, &powermeter_main_style );

    lv_style_copy( &powermeter_id_style, APP_STYLE );
    lv_style_set_text_font( &powermeter_id_style, LV_STATE_DEFAULT, &Ubuntu_16px);

    lv_obj_t * exit_btn = wf_add_exit_button( powermeter_main_tile, SYSTEM_ICON_STYLE );
    lv_obj_align(exit_btn, powermeter_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    lv_obj_t * setup_btn = wf_add_setup_button( powermeter_main_tile, enter_powermeter_setup_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align(setup_btn, powermeter_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );

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

#ifdef NATIVE_64BIT
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, true, NULL);
    mosquitto_message_callback_set(mosq, callback );
    mosquitto_connect_callback_set(mosq, on_connect);
#else
    powermeter_mqtt_client.setCallback( callback );
    powermeter_mqtt_client.setBufferSize( 512 );
#endif
    wifictl_register_cb( WIFICTL_CONNECT_IP | WIFICTL_OFF_REQUEST | WIFICTL_OFF | WIFICTL_DISCONNECT , powermeter_wifictl_event_cb, "powermeter" );
    styles_register_cb( STYLE_CHANGE, powermeter_style_change_event_cb, "powermeter style event ");
    // create an task that runs every secound
    _powermeter_main_task = lv_task_create( powermeter_main_task, 250, LV_TASK_PRIO_MID, NULL );
}

bool powermeter_style_change_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:  lv_style_copy( &powermeter_main_style, APP_STYLE );
                            lv_style_set_text_font( &powermeter_main_style, LV_STATE_DEFAULT, &Ubuntu_48px);
                            lv_style_copy( &powermeter_id_style, APP_STYLE );
                            lv_style_set_text_font( &powermeter_id_style, LV_STATE_DEFAULT, &Ubuntu_16px);
                            break;
        case STYLE_DARKMODE:
                            break;
        case STYLE_LIGHTMODE:
                            break;
    }
    return( true );
}

bool powermeter_wifictl_event_cb( EventBits_t event, void *arg ) {
    powermeter_config_t *powermeter_config = powermeter_get_config();

    switch( event ) {
        case WIFICTL_CONNECT_IP:    
#ifdef NATIVE_64BIT
                                    if ( powermeter_config->autoconnect ) {
                                        mosquitto_username_pw_set( mosq, powermeter_config->user, powermeter_config->password );
                                        int rc = mosquitto_connect( mosq, powermeter_config->server, powermeter_config->port, 60 );
                                        log_i("connect to : %s", powermeter_config->server );
                                      	if(rc != MOSQ_ERR_SUCCESS){
                                            mosquitto_destroy(mosq);
                                            log_i("Error: %s\n", mosquitto_strerror(rc) );
                                            return 1;
                                    	}
                                    }
#else
                                    if ( powermeter_config->autoconnect ) {
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
#endif                               
                                    break;
        case WIFICTL_OFF_REQUEST:
        case WIFICTL_OFF:
        case WIFICTL_DISCONNECT:    
#ifdef NATIVE_64BIT

#else
                                    if ( powermeter_mqtt_client.connected() ) {
                                        log_i("disconnect from mqtt server %s", powermeter_config->server );
                                        powermeter_mqtt_client.disconnect();
                                        app_hide_indicator( powermeter_get_app_icon() );
                                        widget_hide_indicator( powermeter_get_widget_icon() );
                                        widget_set_label( powermeter_get_widget_icon(), "n/a" );
                                    }
#endif
                                    break;
    }
    return( true );
}

static void enter_powermeter_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( powermeter_get_app_setup_tile_num(), LV_ANIM_ON );
                                        statusbar_hide( true );
                                        break;
    }
}

void powermeter_main_task( lv_task_t * task ) {
    // put your code her
#ifdef NATIVE_64BIT
    mosquitto_loop( mosq, 60, 10 );
#else
    powermeter_mqtt_client.loop();
#endif
}