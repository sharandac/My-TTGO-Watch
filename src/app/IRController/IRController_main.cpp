/****************************************************************************
 *  IRController_main.cpp
 *  Copyright  2020  David Stewart
 *  Email: genericsoftwaredeveloper@gmail.com
 *
 *  Requires Library: IRremoteESP8266 by David Conran
 *
 *  Remote codes are stored in IRController_main.h, currently only a handful
 *  of Samsung TV remote codes are defined.
 *
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
 * 
 */
#include "config.h"
#include <TTGO.h>

#include "IRController.h"
#include "IRController_main.h"

#include <IRremoteESP8266.h>
#include <IRsend.h>

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"


lv_obj_t *IRController_main_tile = NULL;
lv_style_t IRController_main_style;

//lv_task_t * _IRController_task; //unused

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
//LV_IMG_DECLARE(refresh_32px); //unused
LV_FONT_DECLARE(Ubuntu_32px);

IRsend irsend(13);

static void exit_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event );
//static void enter_IRController_setup_event_cb( lv_obj_t * obj, lv_event_t event );

static void mute_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void pwr_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void source_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void volup_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void voldn_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void rExit_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event );


void IRController_main_setup( uint32_t tile_num ) {

    IRController_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &IRController_main_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( IRController_main_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px );
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &IRController_main_style );
    lv_obj_align( exit_btn, IRController_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_IRController_main_event_cb );

    /*//No use for this just yet
    lv_obj_t * setup_btn = lv_imgbtn_create( IRController_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &IRController_main_style );
    lv_obj_align(setup_btn, IRController_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_IRController_setup_event_cb );
    */
    
    //Top Left, power button
    lv_obj_t *IRController_main_pwr_btn = NULL;
    IRController_main_pwr_btn = lv_btn_create( IRController_main_tile, NULL);  
    lv_obj_set_size( IRController_main_pwr_btn, 70, 40);
    lv_obj_set_event_cb( IRController_main_pwr_btn, pwr_IRController_main_event_cb );
    lv_obj_add_style( IRController_main_pwr_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( IRController_main_pwr_btn, NULL, LV_ALIGN_CENTER, -80, -90 );
    lv_obj_t *IRController_main_pwr_label = lv_label_create( IRController_main_pwr_btn, NULL);
    lv_label_set_text( IRController_main_pwr_label, "Power");
    
    //Middle Left, mute button
    lv_obj_t *IRController_main_mute_btn = NULL;
    IRController_main_mute_btn = lv_btn_create( IRController_main_tile, NULL);  
    lv_obj_set_event_cb( IRController_main_mute_btn, mute_IRController_main_event_cb );
    lv_obj_set_size( IRController_main_mute_btn, 70, 40);
    lv_obj_add_style( IRController_main_mute_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( IRController_main_mute_btn, NULL, LV_ALIGN_CENTER, -80, -30 );
    lv_obj_t *IRController_main_mute_label = lv_label_create( IRController_main_mute_btn, NULL);
    lv_label_set_text( IRController_main_mute_label, "Mute");
    
    //Bottom Left, Source button
    lv_obj_t *IRController_main_source_btn = NULL;
    IRController_main_source_btn = lv_btn_create( IRController_main_tile, NULL);  
    lv_obj_set_event_cb( IRController_main_source_btn, source_IRController_main_event_cb );
    lv_obj_set_size( IRController_main_source_btn, 70, 40);
    lv_obj_add_style( IRController_main_source_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( IRController_main_source_btn, NULL, LV_ALIGN_CENTER, -80, 30 );
    lv_obj_t *IRController_main_source_label = lv_label_create( IRController_main_source_btn, NULL);
    lv_label_set_text( IRController_main_source_label, "Source");
    
    //Top Right, Volume + button
    lv_obj_t *IRController_main_volup_btn = NULL;
    IRController_main_volup_btn = lv_btn_create( IRController_main_tile, NULL);  
    lv_obj_set_event_cb( IRController_main_volup_btn, volup_IRController_main_event_cb );
    lv_obj_set_size( IRController_main_volup_btn, 70, 40);
    lv_obj_add_style( IRController_main_volup_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( IRController_main_volup_btn, NULL, LV_ALIGN_CENTER, 10, -90 );
    lv_obj_t *IRController_main_volup_label = lv_label_create(IRController_main_volup_btn, NULL);
    lv_label_set_text( IRController_main_volup_label, "Vol +");
    
    //Middle Right, Volume - button
    lv_obj_t *IRController_main_voldn_btn = NULL;
    IRController_main_voldn_btn = lv_btn_create( IRController_main_tile, NULL);  
    lv_obj_set_event_cb( IRController_main_voldn_btn, voldn_IRController_main_event_cb );
    lv_obj_set_size( IRController_main_voldn_btn, 70, 40);
    lv_obj_add_style( IRController_main_voldn_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( IRController_main_voldn_btn, NULL, LV_ALIGN_CENTER, 10, -30 );
    lv_obj_t *IRController_main_voldn_label = lv_label_create( IRController_main_voldn_btn, NULL);
    lv_label_set_text( IRController_main_voldn_label, "Vol -");

    //Bottom Right, Exit menu button (This is exit for the TV, not the app)
    lv_obj_t *IRController_main_rExit_btn = NULL;
    IRController_main_rExit_btn = lv_btn_create( IRController_main_tile, NULL);  
    lv_obj_set_event_cb( IRController_main_rExit_btn, rExit_IRController_main_event_cb );
    lv_obj_set_size( IRController_main_rExit_btn, 70, 40);
    lv_obj_add_style( IRController_main_rExit_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align(IRController_main_rExit_btn, NULL, LV_ALIGN_CENTER, 10, 30 );
    lv_obj_t *IRController_main_rExit_label = lv_label_create( IRController_main_rExit_btn, NULL);
    lv_label_set_text( IRController_main_rExit_label, "Exit");    


    lv_style_set_text_opa( &IRController_main_style, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_text_font( &IRController_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_obj_t *app_label = lv_label_create( IRController_main_tile, NULL);
    lv_label_set_text( app_label, "IR\nController");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, &IRController_main_style );
    lv_obj_align( app_label, IRController_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    
}

/*//Not yet in use
static void enter_IRController_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( IRController_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}
*/

static void exit_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        pinMode(13, INPUT); //Leave this tristated when we leave
                                        mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}
static void pwr_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        pinMode(13, OUTPUT);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off            
                                        irsend.sendRaw(S_pwr, 68, 38);
                                        delay(50);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off
                                        break;
    }
}
static void mute_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        pinMode(13, OUTPUT);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off            
                                        irsend.sendRaw(S_mute, 68, 38);
                                        delay(50);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off
                                        break;
    }
}

static void source_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        pinMode(13, OUTPUT);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off            
                                        irsend.sendRaw(S_scr, 68, 38);
                                        delay(50);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off
                                        break;
    }
}
static void volup_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        pinMode(13, OUTPUT);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off            
                                        irsend.sendRaw(S_vup, 68, 38);
                                        delay(50);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off
                                        break;
    }
}

static void voldn_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        pinMode(13, OUTPUT);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off            
                                        irsend.sendRaw(S_vdown, 68, 38);
                                        delay(50);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off
                                        break;
    }
}
static void rExit_IRController_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        pinMode(13, OUTPUT);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off            
                                        irsend.sendRaw(S_exit, 68, 38);
                                        delay(50);
                                        digitalWrite(13, LOW); //No Current Limiting so keep it off
                                        break;
    }
}

