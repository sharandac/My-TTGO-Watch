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

#include "osmand_app.h"
#include "osmand_app_main.h"

#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

#include "hardware/display.h"
#include "hardware/blectl.h"
#include "hardware/json_psram_allocator.h"
#include "hardware/powermgm.h"

lv_obj_t *osmand_app_main_tile = NULL;
lv_style_t osmand_app_main_style;
lv_style_t osmand_app_distance_style;

lv_obj_t *osmand_app_direction_img = NULL;
lv_obj_t *osmand_app_distance_label = NULL;
lv_obj_t *osmand_app_info_label = NULL;

lv_task_t * _osmand_app_task;

static bool osmand_active = false;
static bool osmand_block_return_maintile = false;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(ahead_128px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(turn_left_128px);
LV_IMG_DECLARE(turn_right_128px);
LV_IMG_DECLARE(slightly_left_128px);
LV_IMG_DECLARE(slightly_right_128px);
LV_IMG_DECLARE(sharply_left_128px);
LV_IMG_DECLARE(sharply_right_128px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

struct direction_t direction[] = {
    // english directions
    { "ahead", "", &ahead_128px },
    { "left", "slightly", &slightly_left_128px },
    { "right", "slightly", &slightly_right_128px },
    { "left", "sharply", &sharply_left_128px },
    { "right", "sharply", &sharply_right_128px },
    { "turn left", "", &turn_left_128px },
    { "turn right", "", &turn_right_128px },
    // german direction
    { "Geradeaus", "", &ahead_128px },
    { "links abbiegen", "halb", &slightly_left_128px },
    { "rechts abbiegen", "halb", &slightly_right_128px },
    { "links abbiegen", "scharf", &sharply_left_128px },
    { "rechts abbiegen", "scharf", &sharply_right_128px },
    { "links abbiegen", "", &turn_left_128px },
    { "rechts abbiegen", "", &turn_right_128px },
    // french direction
    { "Avancez", "", &ahead_128px },
    { "gauche et continuez", "vers la", &slightly_left_128px },
    { "droite et continuez", "vers la", &slightly_right_128px },
    { "gauche et continuez", "franchement", &sharply_left_128px },
    { "droite et continuez", "franchement", &sharply_right_128px },
    { "gauche et continuez", "", &turn_left_128px },
    { "droite et continuez", "", &turn_right_128px },
        // italian direction
    { "Avanti", "", &ahead_128px },
    { "sinistra", "leggermente", &slightly_left_128px },
    { "destra", "leggermente", &slightly_right_128px },
    { "sinistra", "bruscamente", &sharply_left_128px },
    { "destra", "bruscamente", &sharply_right_128px },
    { "gira a sinistra", "", &turn_left_128px },
    { "gira a destra", "", &turn_right_128px },
    { "", "", NULL }
};

static void exit_osmand_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
bool osmand_bluetooth_message_event_cb( EventBits_t event, void *arg );
void osmand_bluetooth_message_msg_pharse( const char* msg );
const lv_img_dsc_t *osmand_find_direction_img( const char * msg );
void osmand_activate_cb( void );
void osmand_hibernate_cb( void );
void osmand_app_task( lv_task_t * task );

void osmand_app_main_setup( uint32_t tile_num ) {

    osmand_app_main_tile = mainbar_get_tile_obj( tile_num );

    lv_style_copy( &osmand_app_main_style, mainbar_get_style() );
    lv_style_set_bg_color( &osmand_app_main_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &osmand_app_main_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &osmand_app_main_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &osmand_app_main_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_obj_add_style( osmand_app_main_tile, LV_OBJ_PART_MAIN, &osmand_app_main_style );

    lv_style_copy( &osmand_app_distance_style, &osmand_app_main_style );
    lv_style_set_text_font( &osmand_app_distance_style, LV_STATE_DEFAULT, &Ubuntu_32px);

    lv_obj_t * exit_btn = lv_imgbtn_create( osmand_app_main_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &cancel_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &osmand_app_main_style );
    lv_obj_align( exit_btn, osmand_app_main_tile, LV_ALIGN_IN_TOP_RIGHT, -10, 10 );
    lv_obj_set_event_cb( exit_btn, exit_osmand_app_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( osmand_app_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &osmand_app_main_style );
    lv_obj_align(setup_btn, osmand_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );
//    lv_obj_set_event_cb( setup_btn, enter_example_app_setup_event_cb );

    osmand_app_direction_img = lv_img_create( osmand_app_main_tile, NULL );
    lv_img_set_src( osmand_app_direction_img, &ahead_128px );
    lv_obj_align( osmand_app_direction_img, osmand_app_main_tile, LV_ALIGN_IN_TOP_MID, 0, 32 );

    osmand_app_distance_label = lv_label_create( osmand_app_main_tile, NULL);
    lv_obj_add_style( osmand_app_distance_label, LV_OBJ_PART_MAIN, &osmand_app_distance_style  );
    lv_label_set_text( osmand_app_distance_label, "n/a");
    lv_obj_align( osmand_app_distance_label, osmand_app_direction_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    osmand_app_info_label = lv_label_create( osmand_app_main_tile, NULL);
    lv_obj_add_style( osmand_app_info_label, LV_OBJ_PART_MAIN, &osmand_app_main_style  );
    lv_label_set_text( osmand_app_info_label, "no bluetooth connection");
    lv_obj_align( osmand_app_info_label, osmand_app_distance_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    mainbar_add_tile_activate_cb( tile_num, osmand_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, osmand_hibernate_cb );

    blectl_register_cb( BLECTL_MSG | BLECTL_CONNECT | BLECTL_DISCONNECT , osmand_bluetooth_message_event_cb, "OsmAnd main" );
}

static void exit_osmand_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            mainbar_jump_to_maintile( LV_ANIM_OFF );
            break;
    }
}

bool osmand_bluetooth_message_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG:            
            osmand_bluetooth_message_msg_pharse( (const char*)arg );
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

void osmand_bluetooth_message_msg_pharse( const char* msg ) {
    if ( osmand_active == false ) {
        return;
    }

    SpiRamJsonDocument doc( strlen( msg ) * 2 );

    DeserializationError error = deserializeJson( doc, msg );
    if ( error ) {
        log_e("bluetooth message deserializeJson() failed: %s", error.c_str() );
    }
    else  {
        if ( doc["t"] && doc["src"] && doc["title"] ) {
            if ( !strcmp( doc["t"], "notify" ) && !strcmp( doc["src"], "OsmAnd" ) ) {
                if ( strstr( doc["title"], "?") ) {
                    const char * distance = doc["title"];
                    char * direction = strstr( doc["title"], "?");
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

    }
    lv_obj_invalidate( lv_scr_act() );
    doc.clear();
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
    _osmand_app_task = lv_task_create(osmand_app_task, 1000,  LV_TASK_PRIO_LOWEST, NULL );
}

void osmand_hibernate_cb( void ) {
    osmand_active = false;
    bluetooth_message_enable();
    display_set_block_return_maintile( osmand_block_return_maintile );
    lv_task_del( _osmand_app_task );
}

void osmand_app_task( lv_task_t * task ) {
    // put your code her
}