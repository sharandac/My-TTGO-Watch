/****************************************************************************
 *  utilities.cpp
 *  Copyright  2020  David Stewart
 *  Email: genericsoftwaredeveloper@gmail.com
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch
 * 
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
#include "utilities.h"
#include "esp_system.h"//Needed for reset types
#include <Arduino.h>

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"

#include "hardware/motor.h"
#include "hardware/display.h"



lv_obj_t *utilities_tile=NULL;
lv_style_t utilities_style;
uint32_t utilities_tile_num;

lv_obj_t *reboot_btn = NULL;
lv_obj_t *poweroff_btn = NULL;

lv_obj_t *format_spiffs_btn = NULL;

lv_obj_t *SpiffsWarningBox = NULL;

static lv_style_t style_modal;


LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(utilities_64px);

static void enter_utilities_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_utilities_event_cb( lv_obj_t * obj, lv_event_t event );

static void SpiffsWarningBox_event_handler( lv_obj_t * obj, lv_event_t event );
static void format_SPIFFS_utilities_event_cb( lv_obj_t * obj, lv_event_t event );
static void format_SPIFFS(void);

static void reboot_utilities_event_cb( lv_obj_t * obj, lv_event_t event );
static void poweroff_utilities_event_cb( lv_obj_t * obj, lv_event_t event );


void utilities_tile_setup( void ) {
    // get an app tile and copy mainstyle
    utilities_tile_num = mainbar_add_app_tile( 1, 1, "Utilities setup" );
    utilities_tile = mainbar_get_tile_obj( utilities_tile_num );
    lv_style_copy( &utilities_style, mainbar_get_style() );
    lv_style_set_bg_color( &utilities_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &utilities_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &utilities_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( utilities_tile, LV_OBJ_PART_MAIN, &utilities_style );

    icon_t *utilities_setup_icon = setup_register( "Utilities", &utilities_64px, enter_utilities_event_cb );
    setup_hide_indicator( utilities_setup_icon );

    lv_obj_t *exit_btn = lv_imgbtn_create( utilities_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &utilities_style );
    lv_obj_align( exit_btn, utilities_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_utilities_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( utilities_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &utilities_style  );
    lv_label_set_text( exit_label, "System Utilities");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    //Spiffs:
    //Add button for dump spiffs details to serial including config files
    //Add button for clear all spiffs settings
    //Add button for reformat spiffs

    //Add button for SPIFFS format
    format_spiffs_btn = lv_btn_create( utilities_tile, NULL);
    lv_obj_set_event_cb( format_spiffs_btn, format_SPIFFS_utilities_event_cb );
    lv_obj_set_size( format_spiffs_btn, 80, 60);
    lv_obj_add_style( format_spiffs_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( format_spiffs_btn, utilities_tile, LV_ALIGN_CENTER, 0, -15);
    lv_obj_t *format_spiffs_btn_label = lv_label_create( format_spiffs_btn, NULL );
    lv_label_set_text( format_spiffs_btn_label, "Format\nSPIFFS");
    
    //Add button for reboot
    reboot_btn = lv_btn_create( utilities_tile, NULL);
    lv_obj_set_size(reboot_btn, 70, 40);
    lv_obj_set_event_cb( reboot_btn, reboot_utilities_event_cb );
    lv_obj_add_style( reboot_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( reboot_btn, utilities_tile, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5 );
    lv_obj_t *reboot_btn_label = lv_label_create( reboot_btn, NULL );
    lv_label_set_text( reboot_btn_label, "Reboot");

    // Add button for poweroff, Equivalent to holding the power button till the hard poweroff state,
    // 300uA power consumption!
    poweroff_btn = lv_btn_create( utilities_tile, NULL);
    lv_obj_set_size(poweroff_btn, 80, 40);
    lv_obj_set_event_cb( poweroff_btn, poweroff_utilities_event_cb );
    lv_obj_add_style( poweroff_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( poweroff_btn, utilities_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5 );
    lv_obj_t *poweroff_btn_label = lv_label_create( poweroff_btn, NULL );
    lv_label_set_text( poweroff_btn_label, "Poweroff");
    
    lv_obj_t *last_reboot_label = lv_label_create( utilities_tile, NULL);
    lv_obj_add_style( last_reboot_label, LV_OBJ_PART_MAIN, &utilities_style  );
    lv_label_set_text( last_reboot_label, "Last Reboot Reason:");
    lv_obj_align( last_reboot_label, format_spiffs_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
    
    lv_obj_t *last_reason_label = lv_label_create( utilities_tile, NULL);
    lv_obj_add_style( last_reason_label, LV_OBJ_PART_MAIN, &utilities_style  );
    lv_label_set_text( last_reason_label, "");  
    
    //Get the reason for the last reset, this could be moved into a dedicated function....
    esp_reset_reason_t why = esp_reset_reason();
    switch (why){
      case (ESP_RST_UNKNOWN):
                                        lv_label_set_text( last_reason_label, "Unknown");
                                        break;
      case (ESP_RST_POWERON):
                                        lv_label_set_text( last_reason_label, "Power On");//Power On
                                        break;
      case (ESP_RST_EXT):
                                        lv_label_set_text( last_reason_label, "External\nPin");
                                        break;
      case (ESP_RST_SW):
                                        lv_label_set_text( last_reason_label, "Software\nReset");
                                        break;
      case (ESP_RST_PANIC):
                                        lv_label_set_text( last_reason_label, "Exception\nor Panic");
                                        break;
      case (ESP_RST_INT_WDT):
                                        lv_label_set_text( last_reason_label, "Interrupt\nWatchdog");
                                        break;
      case (ESP_RST_TASK_WDT):
                                        lv_label_set_text( last_reason_label, "Task\nWatchdog");
                                        break;
      case (ESP_RST_WDT):
                                        lv_label_set_text( last_reason_label, "Other\nWatchdogs");
                                        break;
      case (ESP_RST_DEEPSLEEP):
                                        lv_label_set_text( last_reason_label, "Exit Deep\nSleep");
                                        break;
      case (ESP_RST_BROWNOUT):
                                        lv_label_set_text( last_reason_label, "Brownout");
                                        break;
      case (ESP_RST_SDIO):
                                        lv_label_set_text( last_reason_label, "Rst by\nSDIO");
                                        break;
      default:
                                        lv_label_set_text( last_reason_label, "No Reason\nReturned");
                                        break;
    }
    lv_label_set_align( last_reason_label, LV_LABEL_ALIGN_CENTER );
    lv_obj_align( last_reason_label, last_reboot_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );//Now that the text has changed, align it.
}

static void enter_utilities_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( utilities_tile_num, LV_ANIM_OFF );
                                        break;
    }
}


static void exit_utilities_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}
//********************************SPIFFS stuff

static void SpiffsWarningBox_event_handler( lv_obj_t * obj, lv_event_t event ){
    if( event == LV_EVENT_DELETE && obj == SpiffsWarningBox ) {
        /* Delete the parent modal background */
        lv_obj_del_async( lv_obj_get_parent( SpiffsWarningBox ) );
        SpiffsWarningBox = NULL; /* happens before object is actually deleted! */
    }
    else if( event == LV_EVENT_VALUE_CHANGED ) {
        if ( !strcmp( lv_msgbox_get_active_btn_text(obj), "Apply" ) ){
            format_SPIFFS();
        }
        lv_msgbox_start_auto_close( SpiffsWarningBox, 0 );
    }
}

static void format_SPIFFS_utilities_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):           
                                    static const char * btns[] = {"Apply", "Cancel", ""};
    
                                    //Setup shading of background
                                    lv_style_set_bg_color( &style_modal, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x20,0x20,0x20));
                                    lv_style_set_bg_opa( &style_modal, LV_STATE_DEFAULT, LV_OPA_80);
            
                                    //The click absorbing screen behind the msgbox
                                    lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
                                    lv_obj_reset_style_list( obj, LV_OBJ_PART_MAIN);
                                    lv_obj_add_style( obj, LV_OBJ_PART_MAIN, &style_modal );
                                    lv_obj_set_pos( obj, 0, 0);
                                    lv_obj_set_size( obj, LV_HOR_RES, LV_VER_RES);
                                    
            
                                    //If you change the message below you need to recalculate the character buffer size!
                                    char temp[72]=""; //65 characters - 2 (%d) + 8 (16mb in bytes=16777216) + 1 = 72 characters
                                    snprintf( temp, sizeof( temp ), "Confirm reformat of SPIFFS, and reset settings?\n(Used bytes: %d)", SPIFFS.usedBytes() );
                                    
                                    SpiffsWarningBox = lv_msgbox_create(obj, NULL);
                                    lv_msgbox_set_text(SpiffsWarningBox, temp);
                                    lv_msgbox_add_btns(SpiffsWarningBox, btns);
                                    lv_obj_set_width(SpiffsWarningBox, 240);
                                    lv_obj_set_event_cb(SpiffsWarningBox, SpiffsWarningBox_event_handler);
                                    lv_obj_align(SpiffsWarningBox, NULL, LV_ALIGN_CENTER, 0 ,0);
                                    break;
    }
}

static void format_SPIFFS(void){
    log_i("SPIFFS Format by User");
    motor_vibe(20);
    delay(20);
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->stopLvglTick();
    SPIFFS.end();
    log_i("SPIFFS unmounted!");
    delay(100);
    SPIFFS.format();
    log_i("SPIFFS format complete!");
    ttgo->startLvglTick();
    motor_vibe(20);
    delay(100);
    bool newmount_attempt = SPIFFS.begin();
    if (!newmount_attempt){
        log_e("SPIFFS New Mount failed, rebooting");
        delay(1000);
        ESP.restart();
    }
}
//********************************Power stuff
static void reboot_utilities_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        log_i("System reboot by user");
                                        motor_vibe(20);
                                        delay(20);
                                        display_standby();

                                        TTGOClass *ttgo = TTGOClass::getWatch();
                                        ttgo->stopLvglTick();
                                        SPIFFS.end();
                                        log_i("SPIFFS unmounted!");
                                        delay(500);
                                        ESP.restart();
                                        break;
    }
}


static void poweroff_utilities_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        log_i("System poweroff by user");
                                        motor_vibe(20);
                                        delay(20);
                                        
                                        TTGOClass *ttgo = TTGOClass::getWatch();
                                        ttgo->stopLvglTick();
                                        SPIFFS.end();
                                        log_i("SPIFFS unmounted!");
                                        delay(500);
                                        ttgo->power->shutdown();
    }
}
