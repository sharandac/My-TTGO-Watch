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
#include "kodi_remote_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/wifictl.h"
#include "utils/json_psram_allocator.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>

    using namespace std;
    #define String string
#else
    #include <Arduino.h>
    #include "HTTPClient.h"
#endif

static const char* buttons[] = {"Info","Up","Context","\n","Left","OK","Right","\n","Back","Down","Exit",""};
bool kodi_remote_state = false;
volatile bool kodi_remote_open_state = false;
volatile bool kodi_remote_play_state = false;
uint32_t kodi_remote_id = 0;
int16_t kodi_remote_videoplayer_id = 0;
int16_t kodi_remote_audioplayer_id = 0;
int16_t kodi_remote_pictureplayer_id = 0;
static uint64_t nextmillis = 0;

lv_obj_t *kodi_remote_player_main_tile = NULL;
lv_obj_t *kodi_remote_control_main_tile = NULL;

lv_task_t * _kodi_remote_app_task;

lv_obj_t *kodi_remote_play = NULL;
lv_obj_t *kodi_remote_prev = NULL;
lv_obj_t *kodi_remote_next = NULL;
lv_obj_t *kodi_remote_title = NULL;
lv_obj_t *kodi_remote_artist = NULL;

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

static void kodi_remote_setup_activate_callback ( void );
static void kodi_remote_setup_hibernate_callback ( void );
static void exit_kodi_remote_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_kodi_remote_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static bool kodi_remote_main_wifictl_event_cb( EventBits_t event, void *arg );
static void kodi_remote_volume_up_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_volume_down_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_play_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_next_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_prev_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_button_event_cb( lv_obj_t * obj, lv_event_t event );
static void kodi_remote_control_button(char cmd);

int16_t kodi_remote_get_active_player_id();
void kodi_remote_get_active_player_state();
void kodi_remote_get_active_players();
void kodi_remote_refresh();
int kodi_remote_publish(const char* method, const char* params, SpiRamJsonDocument* doc = nullptr);
void kodi_remote_app_task( lv_task_t * task );

void kodi_remote_app_main_setup( uint32_t tile_num ) {

    // Player Tile
    mainbar_add_tile_activate_cb( tile_num, kodi_remote_setup_activate_callback );
    mainbar_add_tile_hibernate_cb( tile_num, kodi_remote_setup_hibernate_callback );
    kodi_remote_player_main_tile = mainbar_get_tile_obj( tile_num );

    lv_obj_t * exit_btn_player = wf_add_exit_button( kodi_remote_player_main_tile, exit_kodi_remote_main_event_cb );
    lv_obj_align(exit_btn_player, kodi_remote_player_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    lv_obj_t * setup_btn_player = wf_add_setup_button( kodi_remote_player_main_tile, enter_kodi_remote_setup_event_cb );
    lv_obj_align(setup_btn_player, kodi_remote_player_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING, -THEME_PADDING );

    kodi_remote_play = wf_add_image_button( kodi_remote_player_main_tile, play_64px, kodi_remote_play_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( kodi_remote_play, kodi_remote_player_main_tile, LV_ALIGN_CENTER, 0, -20 );

    kodi_remote_next = wf_add_image_button( kodi_remote_player_main_tile, next_32px, kodi_remote_next_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( kodi_remote_next, kodi_remote_play, LV_ALIGN_OUT_RIGHT_MID, 32, 0 );

    kodi_remote_prev = wf_add_image_button( kodi_remote_player_main_tile, prev_32px, kodi_remote_prev_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( kodi_remote_prev, kodi_remote_play, LV_ALIGN_OUT_LEFT_MID, -32, 0 );

    kodi_remote_artist = lv_label_create( kodi_remote_player_main_tile, NULL);
    lv_obj_add_style( kodi_remote_artist, LV_OBJ_PART_MAIN, APP_STYLE  );
    lv_label_set_text( kodi_remote_artist, "");
    lv_label_set_long_mode( kodi_remote_artist, LV_LABEL_LONG_SROLL_CIRC );
    lv_obj_set_width( kodi_remote_artist, lv_disp_get_hor_res( NULL ) - 60 );
    lv_obj_align( kodi_remote_artist, kodi_remote_player_main_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );

    kodi_remote_title = lv_label_create( kodi_remote_player_main_tile, NULL);
    lv_obj_add_style( kodi_remote_title, LV_OBJ_PART_MAIN, APP_STYLE  );
    lv_label_set_text( kodi_remote_title, "");
    lv_label_set_long_mode( kodi_remote_title, LV_LABEL_LONG_SROLL_CIRC );
    lv_obj_set_width( kodi_remote_title, lv_disp_get_hor_res( NULL ) - 20 );
    lv_obj_align( kodi_remote_title, kodi_remote_play, LV_ALIGN_OUT_TOP_MID, 0, -16 );

    lv_obj_t *kodi_remote_speaker = wf_add_image_button( kodi_remote_player_main_tile, sound_32px, NULL, SYSTEM_ICON_STYLE );
    lv_obj_align( kodi_remote_speaker, kodi_remote_play, LV_ALIGN_OUT_BOTTOM_MID, 0, 16 );

    lv_obj_t *kodi_remote_volume_down = wf_add_image_button( kodi_remote_player_main_tile, down_32px, kodi_remote_volume_down_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( kodi_remote_volume_down, kodi_remote_speaker, LV_ALIGN_OUT_LEFT_MID, -32, 0 );

    lv_obj_t *kodi_remote_volume_up = wf_add_image_button( kodi_remote_player_main_tile, up_32px, kodi_remote_volume_up_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( kodi_remote_volume_up, kodi_remote_speaker, LV_ALIGN_OUT_RIGHT_MID, 32, 0 );

    // Control Tile
    kodi_remote_control_main_tile = mainbar_get_tile_obj( tile_num + 1 );

    lv_obj_t * exit_btn_control = wf_add_exit_button( kodi_remote_control_main_tile, exit_kodi_remote_main_event_cb );
    lv_obj_align(exit_btn_control, kodi_remote_control_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    lv_obj_t * setup_btn_control = wf_add_setup_button( kodi_remote_control_main_tile, enter_kodi_remote_setup_event_cb );
    lv_obj_align(setup_btn_control, kodi_remote_control_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING, -THEME_PADDING );

    lv_obj_t * button_matrix = lv_btnmatrix_create(kodi_remote_control_main_tile, NULL);
	lv_obj_add_style(button_matrix, LV_BTNMATRIX_PART_BG, APP_STYLE );
	lv_obj_add_style(button_matrix, LV_BTNMATRIX_PART_BTN, ws_get_button_style() );
	lv_obj_set_pos(button_matrix, 0, 0);
	lv_obj_set_size(button_matrix, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) - THEME_ICON_SIZE );

	lv_btnmatrix_set_map(button_matrix, buttons );
	lv_btnmatrix_set_one_check(button_matrix, false);
    lv_obj_set_event_cb(button_matrix, kodi_remote_button_event_cb);

    mainbar_add_slide_element( button_matrix );

    // callbacks
    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT | WIFICTL_DISCONNECT, kodi_remote_main_wifictl_event_cb, "kodi remote main" );

    // create an task that runs every secound
    _kodi_remote_app_task = lv_task_create( kodi_remote_app_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static void kodi_remote_setup_activate_callback ( void ) {
    kodi_remote_open_state = true;
    nextmillis = 0;
}

static void kodi_remote_setup_hibernate_callback ( void ) {
    kodi_remote_open_state = false;
    nextmillis = 0;
}

static bool kodi_remote_main_wifictl_event_cb( EventBits_t event, void *arg ) {    
    switch( event ) {
        case WIFICTL_CONNECT:       kodi_remote_state = true;
                                    kodi_remote_app_hide_indicator();
                                    break;
        case WIFICTL_DISCONNECT:    kodi_remote_state = false;
                                    kodi_remote_app_set_indicator( ICON_INDICATOR_FAIL );
                                    break;
        case WIFICTL_OFF:           kodi_remote_state = false;
                                    kodi_remote_app_hide_indicator();
                                    break;
    }
    return( true );
}

static void enter_kodi_remote_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( kodi_remote_app_get_app_setup_tile_num(), LV_ANIM_ON );
                                        statusbar_hide( true );
                                        nextmillis = 0;
                                        break;
    }
}

static void exit_kodi_remote_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       kodi_remote_open_state = false;
                                        mainbar_jump_back();
                                        break;
    }
}

static void kodi_remote_play_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            int16_t player = kodi_remote_get_active_player_id();
            if (player < 0) break;

            if( kodi_remote_play_state == true ) {
                lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_RELEASED, &play_64px);
                lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_PRESSED, &play_64px);
                lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_RELEASED, &play_64px);
                lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_PRESSED, &play_64px);
                kodi_remote_play_state = false;
                
                char parameters[24];
                snprintf(parameters, sizeof( parameters ), "{ \"playerid\": %d }", player);
                kodi_remote_publish("Player.PlayPause", parameters);
                nextmillis = 0;
            }
            else {
                lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_RELEASED, &pause_64px);
                lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_PRESSED, &pause_64px);
                lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_RELEASED, &pause_64px);
                lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_PRESSED, &pause_64px);
                kodi_remote_play_state = true;
                
                char parameters[24];
                snprintf(parameters, sizeof( parameters ), "{ \"playerid\": %d }", player);
                kodi_remote_publish("Player.PlayPause", parameters);
                nextmillis = 0;
            }
            break;
    }
}

static void kodi_remote_volume_up_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            kodi_remote_publish("Application.SetVolume", "{ \"volume\": \"increment\" }");
            break;
    }
}

static void kodi_remote_volume_down_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            kodi_remote_publish("Application.SetVolume", "{ \"volume\": \"decrement\" }");
            break;
    }
}

static void kodi_remote_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            int16_t player = kodi_remote_get_active_player_id();
            if (player < 0) break;

            char parameters[42];
            snprintf(parameters, sizeof( parameters ), "{ \"playerid\": %d, \"to\": \"next\" }", player);
            kodi_remote_publish("Player.GoTo", parameters);
            nextmillis = 0;
            break;
    }
}

static void kodi_remote_prev_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            int16_t player = kodi_remote_get_active_player_id();
            if (player < 0) break;

            char parameters[42];
            snprintf(parameters, sizeof( parameters ), "{ \"playerid\": %d, \"to\": \"previous\" }", player);
            kodi_remote_publish("Player.GoTo", parameters);
            nextmillis = 0;
            break;
    }
}

static void kodi_remote_button_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): const char * txt = lv_btnmatrix_get_active_btn_text(obj);
                                        kodi_remote_control_button(txt[0]);
                                        break;
    }
}

static void kodi_remote_control_button(char cmd) {
    switch( cmd ) {
        case 'I': //Info
            kodi_remote_publish("Input.Info", "{}");
            break;
        case 'U': //Up
            kodi_remote_publish("Input.Up", "{}");
            break;
        case 'C': //Context
            kodi_remote_publish("Input.ContextMenu", "{}");
            break;
        case 'L': //Left
            kodi_remote_publish("Input.Left", "{}");
            break;
        case 'O': //OK
            kodi_remote_publish("Input.Select", "{}");
            break;
        case 'R': //Right
            kodi_remote_publish("Input.Right", "{}");
            break;
        case 'B': //Back
            kodi_remote_publish("Input.Back", "{}");
            break;
        case 'D': //Down
            kodi_remote_publish("Input.Down", "{}");
            break;
        case 'E': //Exit
            kodi_remote_publish("Input.Home", "{}");
            break;
    }
}

void kodi_remote_app_task( lv_task_t * task ) {
    if (!kodi_remote_state) return;

    if ( nextmillis < millis() ) {
        if (kodi_remote_open_state || kodi_remote_play_state) {
            nextmillis = millis() + 10000L;
        } else {
            nextmillis = millis() + 60000L;
        }

        kodi_remote_refresh();
    }
}

int16_t kodi_remote_get_active_player_id() {
    int16_t player = -1;
    if (kodi_remote_videoplayer_id >= 0) player = kodi_remote_videoplayer_id;
    if (kodi_remote_audioplayer_id >= 0) player = kodi_remote_audioplayer_id;
    if (kodi_remote_pictureplayer_id >= 0) player = kodi_remote_pictureplayer_id;
    return player;
}

void kodi_remote_get_active_players() {
    SpiRamJsonDocument doc( 1000 );
    int httpcode = kodi_remote_publish("Player.GetActivePlayers", "{}", &doc);
    if (httpcode >= 200 && httpcode < 400) {
        kodi_remote_app_set_indicator( ICON_INDICATOR_OK );

        if (doc.containsKey("result")) {
            kodi_remote_videoplayer_id = -1;
            kodi_remote_audioplayer_id = -1;
            kodi_remote_pictureplayer_id = -1;

            JsonArray players = doc["result"].as<JsonArray>();
            for (JsonObject player : players) {
                if (!player.containsKey("type")) continue;
                if (strncmp(player["type"], "video", 6) == 0) kodi_remote_videoplayer_id = player["playerid"].as<int16_t>();
                if (strncmp(player["type"], "audio", 6) == 0) kodi_remote_audioplayer_id = player["playerid"].as<int16_t>();
                if (strncmp(player["type"], "picture", 8) == 0) kodi_remote_pictureplayer_id = player["playerid"].as<int16_t>();
            }
        }

        doc.clear();
    } else {
        kodi_remote_app_set_indicator( ICON_INDICATOR_FAIL );
        doc.clear();
    }
}

void kodi_remote_get_active_player_state() {
    int16_t player = kodi_remote_get_active_player_id();
    if (player < 0) {
        lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_RELEASED, &play_64px);
        lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_PRESSED, &play_64px);
        lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_RELEASED, &play_64px);
        lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_PRESSED, &play_64px);
        kodi_remote_play_state = false;
        return;
    }

    char parameters[64];
    snprintf(parameters, sizeof( parameters ), "{ \"playerid\": %d, \"properties\": [\"speed\", \"partymode\"] }", player);

    SpiRamJsonDocument doc( 1000 );
    int httpcode = kodi_remote_publish("Player.GetProperties", parameters, &doc);
    if (httpcode >= 200 && httpcode < 400) {
        kodi_remote_app_set_indicator( ICON_INDICATOR_OK );

        if (doc.containsKey("result")) {
            if (doc["result"].containsKey("speed")) {
                if( doc["result"]["speed"].as<uint8_t>() == 0 ) {
                    lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_RELEASED, &play_64px);
                    lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_PRESSED, &play_64px);
                    lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_RELEASED, &play_64px);
                    lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_PRESSED, &play_64px);
                    kodi_remote_play_state = false;
                }
                else {
                    lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_RELEASED, &pause_64px);
                    lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_PRESSED, &pause_64px);
                    lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_RELEASED, &pause_64px);
                    lv_imgbtn_set_src( kodi_remote_play, LV_BTN_STATE_CHECKED_PRESSED, &pause_64px);                    
                    kodi_remote_play_state = true;
                }
            }
        }

        doc.clear();
    } else {
        kodi_remote_app_set_indicator( ICON_INDICATOR_FAIL );
        doc.clear();
    }
}

void kodi_remote_get_active_player_item() {
    int16_t player = kodi_remote_get_active_player_id();
    if (player < 0) {
        lv_label_set_text( kodi_remote_artist, "" );
        lv_label_set_text( kodi_remote_title, "" );
        return;
    }

    char parameters[64];
    snprintf(parameters, sizeof( parameters ), "{ \"playerid\": %d, \"properties\": [\"title\", \"artist\"] }", player);

    SpiRamJsonDocument doc( 1000 );
    int httpcode = kodi_remote_publish("Player.GetItem", parameters, &doc);
    if (httpcode >= 200 && httpcode < 400) {
        kodi_remote_app_set_indicator( ICON_INDICATOR_OK );

        if (doc.containsKey("result")) {
            if (doc["result"].containsKey("item")) {
                if (doc["result"]["item"].containsKey("artist")) {
                    if (doc["result"]["item"]["artist"].is<JsonArray>()) {
                        JsonArray artists = doc["result"]["item"]["artist"].as<JsonArray>();

                        uint8_t num = 0;
                        String artistList;
                        for (JsonVariant artist : artists) {
                            if (num > 0) {
//                                artistList.concat(", ");
                                artistList += ", ";
                            }
//                            artistList.concat( artist.as<const char*>() );
                            artistList += artist.as<const char*>();
                            num++;
                        }
                        
                        lv_label_set_text( kodi_remote_artist, artistList.c_str() );
                    } else {
                        lv_label_set_text( kodi_remote_artist, doc["result"]["item"]["artist"] );
                    }
                    lv_label_set_text( kodi_remote_artist, doc["result"]["item"]["artist"] );
                    lv_obj_align( kodi_remote_artist, kodi_remote_player_main_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );
                } else {
                    lv_label_set_text( kodi_remote_artist, "" );
                }
                
                if (doc["result"]["item"].containsKey("title")) {
                    lv_label_set_text( kodi_remote_title, doc["result"]["item"]["title"] );
                    lv_obj_align( kodi_remote_title, kodi_remote_play, LV_ALIGN_OUT_TOP_MID, 0, -16 );
                } else {
                    lv_label_set_text( kodi_remote_title, "" );
                }
            } else {
                lv_label_set_text( kodi_remote_artist, "" );
                lv_label_set_text( kodi_remote_title, "" );
            }
        }

        doc.clear();
    } else {
        kodi_remote_app_set_indicator( ICON_INDICATOR_FAIL );
        doc.clear();
    }
}

void kodi_remote_refresh() {
    if (!kodi_remote_state) return;

    kodi_remote_get_active_players();
    kodi_remote_get_active_player_state();
    kodi_remote_get_active_player_item();
}

int kodi_remote_publish(const char* method, const char* params, SpiRamJsonDocument* doc) {
    int httpcode = -1;
    if (!kodi_remote_state) return httpcode;

    kodi_remote_config_t *kodi_remote_config = kodi_remote_get_config();
    if (!strlen(kodi_remote_config->host)) return httpcode;

    char url[128] = "";
    snprintf( url, sizeof( url ), "http://%s:%d/jsonrpc", kodi_remote_config->host, kodi_remote_config->port );

#ifdef NATIVE_64BIT
    return( 200 );
#else
    kodi_remote_id++;
    char payload[256] = "";
    snprintf( payload, sizeof( payload ), "{ \"jsonrpc\": \"2.0\", \"method\": \"%s\", \"params\": %s, \"id\": \"%d\" }", method, params, kodi_remote_id );

    HTTPClient publish_client;
    publish_client.useHTTP10( true );
    publish_client.begin( url );
    publish_client.addHeader("Content-Type", "application/json");
    publish_client.setAuthorization( kodi_remote_config->user, kodi_remote_config->pass );
    httpcode = publish_client.POST((uint8_t*)payload, strlen(payload));

    if (httpcode >= 200 && httpcode < 300 && doc != NULL && doc != nullptr) {
        DeserializationError error = deserializeJson( *(doc), publish_client.getStream() );
        if (error) {
            log_e("kodi_remote deserializeJson() failed: %s", error.c_str() );
            publish_client.end();
            return( -2 );
        }
    }

    publish_client.end();
    return httpcode;
#endif
}