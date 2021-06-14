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
#include <TTGO.h>

#include "mqtt_control_app.h"
#include "mqtt_control_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "utils/mqtt/mqtt.h"
#include "utils/alloc.h"

static bool mqtt_control_state = false;

lv_obj_t *mqtt_control_main_tile = NULL;
lv_style_t mqtt_control_main_style;

static void exit_mqtt_control_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_mqtt_control_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static bool mqtt_control_mqtt_event_cb( EventBits_t event, void *arg );
static void mqtt_control_message_cb(char *topic, char *payload, size_t length);

void mqtt_control_main_setup( uint32_t tile_num ) {

    mqtt_control_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &mqtt_control_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = wf_add_exit_button( mqtt_control_main_tile, exit_mqtt_control_main_event_cb, &mqtt_control_main_style );
    lv_obj_align(exit_btn, mqtt_control_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    lv_obj_t * setup_btn = wf_add_setup_button( mqtt_control_main_tile, enter_mqtt_control_setup_event_cb, &mqtt_control_main_style );
    lv_obj_align(setup_btn, mqtt_control_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    
    mqtt_register_cb( MQTT_OFF | MQTT_CONNECTED | MQTT_DISCONNECTED , mqtt_control_mqtt_event_cb, "mqtt control" );
    mqtt_register_message_cb( mqtt_control_message_cb );
}

static bool mqtt_control_mqtt_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case MQTT_OFF:          mqtt_control_state = false;
                                mqtt_control_app_hide_indicator();
                                break;
        case MQTT_CONNECTED:    mqtt_control_state = true;
                                mqtt_control_app_set_indicator( ICON_INDICATOR_OK );
                                break;
        case MQTT_DISCONNECTED: mqtt_control_state = false;
                                mqtt_control_app_set_indicator( ICON_INDICATOR_FAIL );
                                break;
    }
    return( true );
}

static void mqtt_control_message_cb(char *topic, char *payload, size_t length) {
    //TODO: iterate through configured MQTT buttons and labels if any of them needs that message
}

static void enter_mqtt_control_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( mqtt_control_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_mqtt_control_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}