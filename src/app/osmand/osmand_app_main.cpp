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

#include "osmand_app.h"
#include "osmand_app_main.h"

#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/display.h"
#include "hardware/blectl.h"
#include "hardware/powermgm.h"
#include "hardware/timesync.h"

#include "utils/bluejsonrequest.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>

    using namespace std;
    #define String string
#else
    #include <Arduino.h>
#endif

lv_task_t *osmand_app_main_tile_task;

lv_obj_t *osmand_app_main_tile = NULL;
lv_style_t osmand_app_main_style;
lv_style_t osmand_app_time_style;
lv_style_t osmand_app_distance_style;

lv_obj_t *osmand_app_direction_img = NULL;
lv_obj_t *osmand_app_distance_label = NULL;
lv_obj_t *osmand_app_info_label = NULL;
lv_obj_t *osmand_app_time_label = NULL;

static bool osmand_active = false;
static bool osmand_block_return_maintile = false;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(ahead_128px);
LV_IMG_DECLARE(turnleft_128px);
LV_IMG_DECLARE(turnright_128px);
LV_IMG_DECLARE(slightlyleft_128px);
LV_IMG_DECLARE(slightlyright_128px);
LV_IMG_DECLARE(sharplyleft_128px);
LV_IMG_DECLARE(sharplyright_128px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

struct direction_t direction[] = {
    // english directions
    { "ahead", "", &ahead_128px },
    { "left", "slightly", &slightlyleft_128px },
    { "right", "slightly", &slightlyright_128px },
    { "left", "sharply", &sharplyleft_128px },
    { "right", "sharply", &sharplyright_128px },
    { "turn left", "", &turnleft_128px },
    { "turn right", "", &turnright_128px },
    // german direction
    { "Geradeaus", "", &ahead_128px },
    { "links abbiegen", "halb", &slightlyleft_128px },
    { "rechts abbiegen", "halb", &slightlyright_128px },
    { "links abbiegen", "scharf", &sharplyleft_128px },
    { "rechts abbiegen", "scharf", &sharplyright_128px },
    { "links abbiegen", "", &turnleft_128px },
    { "rechts abbiegen", "", &turnright_128px },
    // french direction
    { "Avancez", "", &ahead_128px },
    { "gauche et continuez", "vers la", &slightlyleft_128px },
    { "droite et continuez", "vers la", &slightlyright_128px },
    { "gauche et continuez", "franchement", &sharplyleft_128px },
    { "droite et continuez", "franchement", &sharplyright_128px },
    { "gauche et continuez", "", &turnleft_128px },
    { "droite et continuez", "", &turnright_128px },
        // italian direction
    { "Avanti", "", &ahead_128px },
    { "sinistra", "leggermente", &slightlyleft_128px },
    { "destra", "leggermente", &slightlyright_128px },
    { "sinistra", "bruscamente", &sharplyleft_128px },
    { "destra", "bruscamente", &sharplyright_128px },
    { "gira a sinistra", "", &turnleft_128px },
    { "gira a destra", "", &turnright_128px },
    { "", "", NULL }
};

bool osmand_style_change_event_cb( EventBits_t event, void *arg );
void osmand_app_main_tile_time_update_task( lv_task_t * task );
static void exit_osmand_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
bool osmand_bluetooth_message_event_cb( EventBits_t event, void *arg );
static void osmand_bluetooth_message_msg_pharse( BluetoothJsonRequest &doc );
const lv_img_dsc_t *osmand_find_direction_img( const char * msg );
void osmand_activate_cb( void );
void osmand_hibernate_cb( void );

void osmand_app_main_setup( uint32_t tile_num ) {

    osmand_app_main_tile = mainbar_get_tile_obj( tile_num );

    lv_style_copy( &osmand_app_main_style, APP_STYLE );
    lv_style_set_text_font( &osmand_app_main_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_obj_add_style( osmand_app_main_tile, LV_OBJ_PART_MAIN, &osmand_app_main_style );

    lv_style_copy( &osmand_app_distance_style, &osmand_app_main_style );
    lv_style_set_text_font( &osmand_app_distance_style, LV_STATE_DEFAULT, &Ubuntu_32px);

    lv_style_copy( &osmand_app_time_style, &osmand_app_main_style);
    lv_style_set_text_font( &osmand_app_time_style, LV_STATE_DEFAULT, &Ubuntu_16px);

    osmand_app_time_label = lv_label_create( osmand_app_main_tile , NULL);
    lv_label_set_text(osmand_app_time_label, "00:00");
    lv_obj_reset_style_list( osmand_app_time_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( osmand_app_time_label, LV_OBJ_PART_MAIN, &osmand_app_time_style );
    lv_obj_align( osmand_app_time_label, osmand_app_main_tile, LV_ALIGN_IN_TOP_MID, 0, THEME_PADDING );

    lv_obj_t * exit_btn = wf_add_exit_button( osmand_app_main_tile, exit_osmand_app_main_event_cb );
    lv_obj_align( exit_btn, osmand_app_main_tile, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING, THEME_PADDING );

    osmand_app_direction_img = lv_img_create( osmand_app_main_tile, NULL );
    lv_img_set_src( osmand_app_direction_img, &ahead_128px );
    lv_obj_align( osmand_app_direction_img, osmand_app_main_tile, LV_ALIGN_CENTER, 0, 0 );

    osmand_app_info_label = lv_label_create( osmand_app_main_tile, NULL);
    lv_obj_add_style( osmand_app_info_label, LV_OBJ_PART_MAIN, &osmand_app_main_style  );
    lv_label_set_text( osmand_app_info_label, "no bluetooth connection");
    lv_obj_align( osmand_app_info_label, osmand_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );

    osmand_app_distance_label = lv_label_create( osmand_app_main_tile, NULL);
    lv_obj_add_style( osmand_app_distance_label, LV_OBJ_PART_MAIN, &osmand_app_distance_style  );
    lv_label_set_text( osmand_app_distance_label, "n/a");
    lv_obj_align( osmand_app_distance_label, osmand_app_info_label, LV_ALIGN_OUT_TOP_MID, 0, 0 );

    mainbar_add_tile_activate_cb( tile_num, osmand_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, osmand_hibernate_cb );

    blectl_register_cb( BLECTL_MSG_JSON | BLECTL_CONNECT | BLECTL_DISCONNECT , osmand_bluetooth_message_event_cb, "OsmAnd main" );
    styles_register_cb( STYLE_CHANGE, osmand_style_change_event_cb, "osmand style" );
    osmand_app_main_tile_task = lv_task_create( osmand_app_main_tile_time_update_task, 1000, LV_TASK_PRIO_MID, NULL );
}

bool osmand_style_change_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:  lv_style_copy( &osmand_app_main_style, APP_STYLE );
                            lv_style_set_text_font( &osmand_app_main_style, LV_STATE_DEFAULT, &Ubuntu_16px);

                            lv_style_copy( &osmand_app_distance_style, &osmand_app_main_style );
                            lv_style_set_text_font( &osmand_app_distance_style, LV_STATE_DEFAULT, &Ubuntu_32px);

                            lv_style_copy( &osmand_app_time_style, &osmand_app_main_style);
                            lv_style_set_text_font( &osmand_app_time_style, LV_STATE_DEFAULT, &Ubuntu_16px);        
                            break;
    }
    return( true );
}

void osmand_app_main_tile_time_update_task( lv_task_t * task ) {
    if ( osmand_active ) {
        char time_str[64]="";
        timesync_get_current_timestring( time_str, sizeof( time_str ) );
        lv_label_set_text(osmand_app_time_label, time_str );
        lv_obj_align( osmand_app_time_label, osmand_app_main_tile, LV_ALIGN_IN_TOP_MID, 0, 5 );
    }
}

static void exit_osmand_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            mainbar_jump_back();
            break;
    }
}

bool osmand_bluetooth_message_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG_JSON:            
            osmand_bluetooth_message_msg_pharse( *(BluetoothJsonRequest*)arg );
            break;
        case BLECTL_CONNECT:
            lv_label_set_text( osmand_app_info_label, "wait for OsmAnd msg");
            lv_obj_align( osmand_app_info_label, osmand_app_distance_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            break;
        case BLECTL_DISCONNECT:     
            lv_label_set_text( osmand_app_info_label, "no bluetooth connection");
            lv_obj_align( osmand_app_info_label, osmand_app_distance_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            break;
    }
    return( true );
}

void osmand_bluetooth_message_msg_pharse( BluetoothJsonRequest &doc ) {
    if ( osmand_active == false ) {
        return;
    }

    if ( doc["t"] && doc["src"] && doc["title"] ) {
        /*
         * React to messages from "OsmAnd" and "OsmAnd~"
         */
        if ( !strcmp( doc["t"], "notify" ) && !strncmp( doc["src"], "OsmAnd", 6 ) ) {
            const char * title = doc["title"]; 
            if ( strstr( title, "?") ) {
                const char * distance = doc["title"];
                char * direction = (char *)strstr( title, "?");
                *direction = '\0';
                direction++;
                lv_img_set_src( osmand_app_direction_img, osmand_find_direction_img( (const char*)direction ) );
                lv_obj_align( osmand_app_direction_img, osmand_app_main_tile, LV_ALIGN_IN_TOP_MID, 0, 32 );
                lv_label_set_text( osmand_app_distance_label, distance );
                lv_obj_align( osmand_app_distance_label, osmand_app_direction_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            }
            else {
                lv_label_set_text( osmand_app_info_label, doc["title"] );
                lv_obj_align( osmand_app_info_label, osmand_app_distance_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            }
        }
        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
    }

    lv_obj_invalidate( lv_scr_act() );
}

const lv_img_dsc_t *osmand_find_direction_img( const char * msg ) {
    for ( int i = 0; direction[ i ].img != NULL; i++ ) {
        if ( strstr( msg, direction[ i ].direction ) && strstr( msg, direction[ i ].direction_helper ) ) {
            log_i("hit: %s -> %s", msg, direction[ i ].direction );
            return( direction[ i ].img );
        }
    }
    return( &ahead_128px );
}

void osmand_activate_cb( void ) {
    osmand_active = true;
    bluetooth_message_disable();
    osmand_block_return_maintile = display_get_block_return_maintile();
    display_set_block_return_maintile( true );
}

void osmand_hibernate_cb( void ) {
    osmand_active = false;
    bluetooth_message_enable();
    display_set_block_return_maintile( osmand_block_return_maintile );
}
