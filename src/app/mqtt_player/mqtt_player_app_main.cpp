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

#include "mqtt_player_app.h"
#include "mqtt_player_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "utils/mqtt/mqtt.h"
#include "utils/alloc.h"

static bool mqtt_player_state = false;
static bool mqtt_player_play_state = false;

lv_obj_t *mqtt_player_main_tile = NULL;
lv_style_t mqtt_player_main_style;

lv_task_t * _mqtt_player_task;

lv_obj_t *mqtt_player_play = NULL;
lv_obj_t *mqtt_player_prev = NULL;
lv_obj_t *mqtt_player_next = NULL;
lv_obj_t *mqtt_player_title = NULL;
lv_obj_t *mqtt_player_artist = NULL;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(play_64px);
LV_IMG_DECLARE(pause_64px);
LV_IMG_DECLARE(next_32px);
LV_IMG_DECLARE(prev_32px);
LV_IMG_DECLARE(sound_32px);
LV_IMG_DECLARE(up_32px);
LV_IMG_DECLARE(down_32px);

LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

char mqtt_player_subscribe_topic[34] = "";
static void exit_mqtt_player_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_mqtt_player_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void mqtt_player_volume_up_event_cb( lv_obj_t * obj, lv_event_t event );
static void mqtt_player_volume_down_event_cb( lv_obj_t * obj, lv_event_t event );
static void mqtt_player_play_event_cb( lv_obj_t * obj, lv_event_t event );
static void mqtt_player_next_event_cb( lv_obj_t * obj, lv_event_t event );
static void mqtt_player_prev_event_cb( lv_obj_t * obj, lv_event_t event );
static bool mqtt_player_mqtt_event_cb( EventBits_t event, void *arg );
static void mqtt_player_message_cb(char *topic, char *payload, size_t length);
void mqtt_player_task( lv_task_t * task );

void mqtt_player_main_setup( uint32_t tile_num ) {

    mqtt_player_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &mqtt_player_main_style, ws_get_mainbar_style() );

    lv_style_copy( &mqtt_player_main_style, ws_get_mainbar_style() );
    lv_style_set_text_font( &mqtt_player_main_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_obj_add_style( mqtt_player_main_tile, LV_OBJ_PART_MAIN, &mqtt_player_main_style );

    lv_obj_t * exit_btn = wf_add_exit_button( mqtt_player_main_tile, exit_mqtt_player_main_event_cb, &mqtt_player_main_style );
    lv_obj_align(exit_btn, mqtt_player_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    lv_obj_t * setup_btn = wf_add_setup_button( mqtt_player_main_tile, enter_mqtt_player_setup_event_cb, &mqtt_player_main_style );
    lv_obj_align(setup_btn, mqtt_player_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );

    mqtt_player_play = wf_add_image_button( mqtt_player_main_tile, play_64px, mqtt_player_play_event_cb, &mqtt_player_main_style );
    lv_obj_align( mqtt_player_play, mqtt_player_main_tile, LV_ALIGN_CENTER, 0, -20 );

    mqtt_player_next = wf_add_image_button( mqtt_player_main_tile, next_32px, mqtt_player_next_event_cb, &mqtt_player_main_style );
    lv_obj_align( mqtt_player_next, mqtt_player_play, LV_ALIGN_OUT_RIGHT_MID, 32, 0 );

    mqtt_player_prev = wf_add_image_button( mqtt_player_main_tile, prev_32px, mqtt_player_prev_event_cb, &mqtt_player_main_style );
    lv_obj_align( mqtt_player_prev, mqtt_player_play, LV_ALIGN_OUT_LEFT_MID, -32, 0 );

    mqtt_player_artist = lv_label_create( mqtt_player_main_tile, NULL);
    lv_obj_add_style( mqtt_player_artist, LV_OBJ_PART_MAIN, &mqtt_player_main_style  );
    lv_label_set_text( mqtt_player_artist, "");
    lv_label_set_long_mode( mqtt_player_artist, LV_LABEL_LONG_SROLL_CIRC );
    lv_obj_set_width( mqtt_player_artist, lv_disp_get_hor_res( NULL ) - 60 );
    lv_obj_align( mqtt_player_artist, mqtt_player_main_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );

    mqtt_player_title = lv_label_create( mqtt_player_main_tile, NULL);
    lv_obj_add_style( mqtt_player_title, LV_OBJ_PART_MAIN, &mqtt_player_main_style  );
    lv_label_set_text( mqtt_player_title, "");
    lv_label_set_long_mode( mqtt_player_title, LV_LABEL_LONG_SROLL_CIRC );
    lv_obj_set_width( mqtt_player_title, lv_disp_get_hor_res( NULL ) - 20 );
    lv_obj_align( mqtt_player_title, mqtt_player_play, LV_ALIGN_OUT_TOP_MID, 0, -16 );

    lv_obj_t *mqtt_player_speaker = wf_add_image_button( mqtt_player_main_tile, sound_32px, NULL, &mqtt_player_main_style );
    lv_obj_align( mqtt_player_speaker, mqtt_player_play, LV_ALIGN_OUT_BOTTOM_MID, 0, 16 );

    lv_obj_t *mqtt_player_volume_down = wf_add_image_button( mqtt_player_main_tile, down_32px, mqtt_player_volume_down_event_cb, &mqtt_player_main_style );
    lv_obj_align( mqtt_player_volume_down, mqtt_player_speaker, LV_ALIGN_OUT_LEFT_MID, -32, 0 );

    lv_obj_t *mqtt_player_volume_up = wf_add_image_button( mqtt_player_main_tile, up_32px, mqtt_player_volume_up_event_cb, &mqtt_player_main_style );
    lv_obj_align( mqtt_player_volume_up, mqtt_player_speaker, LV_ALIGN_OUT_RIGHT_MID, 32, 0 );

    mqtt_register_cb( MQTT_OFF | MQTT_CONNECTED | MQTT_DISCONNECTED , mqtt_player_mqtt_event_cb, "mqtt player" );
    mqtt_register_message_cb( mqtt_player_message_cb );

    mqtt_player_config_t *mqtt_player_config = mqtt_player_get_config();
    snprintf( mqtt_player_subscribe_topic, sizeof( mqtt_player_subscribe_topic ), "%s/#", mqtt_player_config->topic_base );

    // create an task that runs every second
    _mqtt_player_task = lv_task_create( mqtt_player_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static bool mqtt_player_mqtt_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case MQTT_OFF:          mqtt_player_state = false;
                                mqtt_player_app_hide_indicator();
                                break;
        case MQTT_CONNECTED:    mqtt_player_state = true;
                                mqtt_subscribe( mqtt_player_subscribe_topic );
                                mqtt_player_app_set_indicator( ICON_INDICATOR_OK );
                                break;
        case MQTT_DISCONNECTED: mqtt_player_state = false;
                                mqtt_player_app_set_indicator( ICON_INDICATOR_FAIL );
                                lv_label_set_text( mqtt_player_artist, "" );
                                lv_label_set_text( mqtt_player_title, "" );
                                break;
    }
    return( true );
}

static void mqtt_player_message_cb(char *topic, char *payload, size_t length) {
    mqtt_player_config_t *mqtt_player_config = mqtt_player_get_config();
    if (strncmp(topic, mqtt_player_config->topic_base, strlen(mqtt_player_config->topic_base) - 1) != 0) return;

    char topic_compare[64] = "";
    char *payload_msg = NULL;
    payload_msg = (char*)CALLOC( length + 1, 1 );
    if ( payload_msg == NULL ) {
        log_e("calloc failed");
        return;
    }
    memcpy( payload_msg, payload, length );

    snprintf( topic_compare, sizeof( topic_compare ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_state );
    if (strncmp(topic, topic_compare, strlen(topic_compare)) == 0) {
        if( !strcmp( payload_msg, "pause" ) || !strcmp( payload_msg, "stop" ) ) {
            lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_RELEASED, &play_64px);
            lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_PRESSED, &play_64px);
            lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_CHECKED_RELEASED, &play_64px);
            lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_CHECKED_PRESSED, &play_64px);
            mqtt_player_play_state = false;
        }
        if( !strcmp( payload_msg, "play" ) ) {
            lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_RELEASED, &pause_64px);
            lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_PRESSED, &pause_64px);
            lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_CHECKED_RELEASED, &pause_64px);
            lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_CHECKED_PRESSED, &pause_64px);                    
            mqtt_player_play_state = true;
        }
    }

    snprintf( topic_compare, sizeof( topic_compare ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_artist );
    if (strncmp(topic, topic_compare, strlen(topic_compare)) == 0) {
        lv_label_set_text( mqtt_player_artist, payload_msg );
        lv_obj_align( mqtt_player_artist, mqtt_player_main_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );
    }

    snprintf( topic_compare, sizeof( topic_compare ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_title );
    if (strncmp(topic, topic_compare, strlen(topic_compare)) == 0) {
        lv_label_set_text( mqtt_player_title, payload_msg );
        lv_obj_align( mqtt_player_title, mqtt_player_play, LV_ALIGN_OUT_TOP_MID, 0, -16 );
    }
}

static void enter_mqtt_player_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( mqtt_player_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_mqtt_player_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

static void mqtt_player_play_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mqtt_player_config_t *mqtt_player_config = mqtt_player_get_config();

            if( mqtt_player_play_state == true ) {
                lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_RELEASED, &play_64px);
                lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_PRESSED, &play_64px);
                lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_CHECKED_RELEASED, &play_64px);
                lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_CHECKED_PRESSED, &play_64px);
                mqtt_player_play_state = false;
                
                char temp[64] = "";
                snprintf( temp, sizeof( temp ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_cmd_pause );
                mqtt_publish(temp, false, "pause\0");
            }
            else {
                lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_RELEASED, &pause_64px);
                lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_PRESSED, &pause_64px);
                lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_CHECKED_RELEASED, &pause_64px);
                lv_imgbtn_set_src( mqtt_player_play, LV_BTN_STATE_CHECKED_PRESSED, &pause_64px);
                mqtt_player_play_state = true;
                
                char temp[64] = "";
                snprintf( temp, sizeof( temp ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_cmd_play );
                mqtt_publish(temp, false, "play\0");
            }
            break;
    }
}

static void mqtt_player_volume_up_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mqtt_player_config_t *mqtt_player_config = mqtt_player_get_config();

            char temp[64] = "";
            snprintf( temp, sizeof( temp ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_cmd_volumeup );
            mqtt_publish(temp, false, "+5\0");
            break;
    }
}

static void mqtt_player_volume_down_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mqtt_player_config_t *mqtt_player_config = mqtt_player_get_config();

            char temp[64] = "";
            snprintf( temp, sizeof( temp ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_cmd_volumedown );
            mqtt_publish(temp, false, "-5\0");
            break;
    }
}

static void mqtt_player_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mqtt_player_config_t *mqtt_player_config = mqtt_player_get_config();

            char temp[64] = "";
            snprintf( temp, sizeof( temp ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_cmd_next );
            mqtt_publish(temp, false, "next\0");
            break;
    }
}

static void mqtt_player_prev_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mqtt_player_config_t *mqtt_player_config = mqtt_player_get_config();

            char temp[64] = "";
            snprintf( temp, sizeof( temp ), "%s/%s", mqtt_player_config->topic_base, mqtt_player_config->topic_cmd_prev );
            mqtt_publish(temp, false, "prev\0");
            break;
    }
}

void mqtt_player_task( lv_task_t * task ) {
    if (!mqtt_player_state) return;
    //TODO: get media length and count it up every second
}