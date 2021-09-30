/****************************************************************************
 *   Apr 17 00:28:11 2021
 *   Copyright  2021  Federico Liuzzi
 *   Email: f.liuzzi02@gmail.com
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

#include "sailing.h"
#include "sailing_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/wifictl.h"
#include "hardware/display.h"

#ifdef NATIVE_64BIT
    #include <string>
    #include "utils/logging.h"
    #include "utils/millis.h"

    using namespace std;
    #define String string
#else
    #include <Arduino.h>
    #include <WiFi.h>
    #include <AsyncUDP.h>
    AsyncUDP *udp = NULL;
#endif

struct pack {
  float heading;
  float track;
  float gspeed;
  float vmg;
  float distance;
};
pack attuale;

lv_obj_t *sailing_main_tile = NULL;
lv_style_t sailing_main_style;
lv_style_t heading_main_style;

lv_obj_t * heading_info_label = NULL;
lv_obj_t * heading_label = NULL;
lv_obj_t * gspeed_label = NULL;
lv_obj_t * vmg_label = NULL;
lv_obj_t * distance_label = NULL;

lv_task_t * _sailing_task;

static volatile bool sailing_app_active = false;
static volatile bool sailing_app_wifi_active = false;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(Ubuntu_48px);
LV_FONT_DECLARE(lv_font_montserrat_28);

void rmc(char dati[]);
void rmb(char dati[]);
void apb(char dati[]);

bool sailing_style_change_event_cb( EventBits_t event, void *arg );
bool sailing_wifictl_event_cb( EventBits_t event, void *arg );
void sailing_activate_cb( void );
void sailing_hibernate_cb( void );
void sailing_app_setup_udp( bool enable );
static void exit_sailing_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_sailing_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void sailing_task( lv_task_t * task );

void sailing_main_setup( uint32_t tile_num ) {

    //Init struct
    attuale.heading = 0.0;
    attuale.gspeed = 0.0;
    attuale.vmg = 0.0;
    attuale.distance = 0.0;

    sailing_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &sailing_main_style, APP_STYLE );
    lv_style_set_text_font( &sailing_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_obj_add_style( sailing_main_tile, LV_OBJ_PART_MAIN, &sailing_main_style );

    // heading style
    lv_style_copy( &heading_main_style, APP_STYLE );
    lv_style_set_text_font( &heading_main_style, LV_STATE_DEFAULT, &Ubuntu_48px);

    lv_obj_t * exit_btn = wf_add_exit_button( sailing_main_tile, exit_sailing_main_event_cb );
    lv_obj_align(exit_btn, sailing_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    lv_obj_t * setup_btn = wf_add_setup_button( sailing_main_tile, enter_sailing_setup_event_cb );
    lv_obj_align(setup_btn, sailing_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING, -THEME_PADDING );

    heading_info_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( heading_info_label, LV_OBJ_PART_MAIN, &heading_main_style );
    lv_label_set_text( heading_info_label, "H =" );
    lv_obj_align( heading_info_label, sailing_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_HEIGHT );
    heading_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( heading_label, LV_OBJ_PART_MAIN, &heading_main_style );
    lv_label_set_text( heading_label, "0°" );
    lv_obj_align( heading_label, sailing_main_tile, LV_ALIGN_IN_TOP_RIGHT, 0, STATUSBAR_HEIGHT );

    lv_obj_t * gspeed_info_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( gspeed_info_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( gspeed_info_label, "Gs =" );
    lv_obj_align( gspeed_info_label, sailing_main_tile, LV_ALIGN_IN_LEFT_MID, 0, -30 );
    gspeed_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( gspeed_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( gspeed_label, "0kt" );
    lv_obj_align( gspeed_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, -30 );

    lv_obj_t * vmg_info_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( vmg_info_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( vmg_info_label, "Vmg =" );
    lv_obj_align( vmg_info_label, sailing_main_tile, LV_ALIGN_IN_LEFT_MID, 0, 10 );
    vmg_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( vmg_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( vmg_label, "0kt" );
    lv_obj_align( vmg_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, 10 );

    lv_obj_t * distance_info_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( distance_info_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( distance_info_label, "Ds =" );
    lv_obj_align( distance_info_label, sailing_main_tile, LV_ALIGN_IN_LEFT_MID, 0, 50 );
    distance_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( distance_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( distance_label, "0nm" );
    lv_obj_align( distance_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, 50 );

    // create an task that runs every secound
    _sailing_task = lv_task_create( sailing_task, 1000, LV_TASK_PRIO_MID, NULL );

    //udp listening
    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT | WIFICTL_DISCONNECT, sailing_wifictl_event_cb, "sailing data" );
    mainbar_add_tile_activate_cb( tile_num, sailing_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, sailing_hibernate_cb );
    styles_register_cb( STYLE_CHANGE, sailing_style_change_event_cb, "sailing style event" );
}

bool sailing_style_change_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:  lv_style_copy( &sailing_main_style, APP_STYLE );
                            lv_style_set_text_font( &sailing_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);    
                            lv_style_copy( &heading_main_style, APP_STYLE );
                            lv_style_set_text_font( &heading_main_style, LV_STATE_DEFAULT, &Ubuntu_48px);
                            break;
    }
    return( true );
}

void sailing_activate_cb( void ) {
    SAILING_INFO_LOG("enter sailing app");
    /**
     * set sailing app active on enter tile
     */
    sailing_app_active = true;
    /**
     * Ensure status bar
     */
    statusbar_hide( false );
    /**
     * if wifi active, enable udp listner
     */
    if ( sailing_app_wifi_active ) {
        sailing_app_setup_udp( true );
    }
}

void sailing_hibernate_cb( void ) {
    SAILING_INFO_LOG("exit sailing app");
    /**
     * set sailing app inactive on exit tile
     */
    sailing_app_active = false;
    /**
     * disable udp listner
     */
    sailing_app_setup_udp( false );
}

bool sailing_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT:   
            /**
             * if sailing app active and wifi comes, start udp listner
             */
            if ( sailing_app_active ) {
                sailing_app_setup_udp( true );
            }
            /**
             * set wifi active
             */
            sailing_app_wifi_active = true;
            break;
        case WIFICTL_DISCONNECT:
            /**
             * set wifi inactive
             */
            sailing_app_wifi_active = false;
            /**
             * disable udp listner
             */
            sailing_app_setup_udp( false );
            break;
    }
    return(true);
}

void sailing_app_setup_udp( bool enable ) {
    if ( enable ) {
#ifdef NATIVE_64BIT

#else
        if ( !udp ) {
            udp = new AsyncUDP;
        }
        /**
         * enable udp listner if not exist
         */
        if( udp->listen( SAILING_UDP_PORT ) ) {
            SAILING_INFO_LOG("UDP Listening on IP: %s", WiFi.localIP().toString().c_str() );
            /**
             * register call back function on packet
             */
            udp->onPacket( [] ( AsyncUDPPacket packet ) {
                char buf[ packet.length() ];
                /**
                 * fill buffer with data
                 */
                for ( int i = 0 ; i < packet.length() ; i++ ) {
                    buf[ i ]= (char)*( packet.data() + i );
                }
                /**
                 * check for data
                 */
                if( String( buf ).startsWith( "$ECRMB" ) )
                    rmb( buf );
                if( String( buf ).startsWith( "$GPRMC" ) )
                    rmc( buf );
                if( String( buf ).startsWith( "$ECAPB" ) )
                    apb( buf );
            });
        }
        else {
            SAILING_ERROR_LOG("create udp listner failed");
        }
#endif
    }
}

static void enter_sailing_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( sailing_get_app_setup_tile_num(), LV_ANIM_ON );
                                        statusbar_hide( true );
                                        break;
    }
}

static void exit_sailing_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        display_set_timeout( 15 );
                                        break;
    }
}

static void sailing_main_update_label()
{
    char heading[10];
    char track[10];
    char gspeed[10];
    char vmg[10];
    char distance[10];
    snprintf(heading, sizeof( heading ), "%.1f°", attuale.heading);
    snprintf(track, sizeof( track ), "%.1f°", attuale.track);
    snprintf(gspeed, sizeof( gspeed ), "%.1fkt", attuale.gspeed);
    snprintf(vmg, sizeof( vmg ), "%.1fkt", attuale.vmg);
    snprintf(distance, sizeof( distance ), "%.1fnm", attuale.distance);

    if( tracking ) {
      lv_label_set_text( heading_info_label, "T =" );
      lv_label_set_text( heading_label, track );
    }
    else {
      lv_label_set_text( heading_info_label, "H =" );
      lv_label_set_text( heading_label, heading );
    }
    lv_obj_align( heading_info_label, sailing_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_HEIGHT );
    lv_obj_align( heading_label, sailing_main_tile, LV_ALIGN_IN_TOP_RIGHT, 0, STATUSBAR_HEIGHT );

    lv_label_set_text( gspeed_label, gspeed );
    lv_obj_align( gspeed_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, -30 );

    lv_label_set_text( vmg_label, vmg );
    lv_obj_align( vmg_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, 10 );

    lv_label_set_text( distance_label, distance );
    lv_obj_align( distance_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, 50 );
}

void sailing_task( lv_task_t * task ) {
    // put your code her
    sailing_main_update_label();
}

//Makes RMC phrase understandable
void rmc(char dati[]){
#ifdef NATIVE_64BIT

#else
    String info[12];
    int i,j = 0;

    for(i = 0; i<12; i++){
        info[i] = "";
        do{
            info[i].concat(dati[j]);
            j++;
        }while(dati[j] != ',');
        j++;
    }

    //passing RMC info to global struct
    attuale.gspeed = info[7].toFloat();
    attuale.heading = info[8].toFloat();
    // Serial.println(attuale.gspeed);
#endif
}

//Makes RMB phrase understandable
void rmb(char dati[]){
#ifdef NATIVE_64BIT

#else
    String info[14];
    int i,j = 0;

    for(i = 0; i<14; i++){
        info[i] = "";
        do{
            info[i].concat(dati[j]);
            j++;
        }while(dati[j] != ',');
        j++;
    }

    //passing RMB info to global struct
    attuale.distance = info[10].toFloat();
    attuale.vmg = info[12].toFloat();
    //Serial.println("Recieved RMB");
#endif
}

//Makes APB phrase understandable
void apb(char dati[]){
#ifdef NATIVE_64BIT

#else  
    String info[14];
    int i,j = 0;

    for(i = 0; i<14; i++){
        info[i] = "";
        do{
            info[i].concat(dati[j]);
            j++;
        }while(dati[j] != ',');
        j++;
    }

    //passing APB info to global struct
    attuale.track = info[13].toFloat();
#endif
}
