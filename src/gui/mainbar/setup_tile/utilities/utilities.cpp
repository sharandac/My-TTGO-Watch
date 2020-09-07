/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#include "utilities.h"
#include "esp_system.h"
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


LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(utilities_64px);

static void enter_utilities_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_utilities_event_cb( lv_obj_t * obj, lv_event_t event );

static void SpiffsWarningBox_event_handler( lv_obj_t * obj, lv_event_t event );
static void format_SPIFFS_utilities_event_cb( lv_obj_t * obj, lv_event_t event );

static void reboot_utilities_event_cb( lv_obj_t * obj, lv_event_t event );
static void poweroff_utilities_event_cb( lv_obj_t * obj, lv_event_t event );


void utilities_tile_setup( void ) {
    // get an app tile and copy mainstyle
    utilities_tile_num = mainbar_add_app_tile( 1, 1 );
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
    lv_obj_align( format_spiffs_btn, NULL, LV_ALIGN_CENTER, 0, -25);
    lv_obj_t *format_spiffs_btn_label = lv_label_create( format_spiffs_btn, NULL );
    lv_label_set_text( format_spiffs_btn_label, "Format SPIFFS");
    
    static const char * btns[] ={"Apply", "Cancel", ""};
    
    lv_obj_t *SpiffsWarningBox = lv_msgbox_create(utilities_tile, NULL);
    lv_msgbox_set_text(SpiffsWarningBox, "Confirm reformat spiffs partition and reset settings?");
    lv_msgbox_add_btns(SpiffsWarningBox, btns);
    lv_obj_set_width(SpiffsWarningBox, 200);
    lv_obj_set_event_cb(SpiffsWarningBox, SpiffsWarningBox_event_handler);
    lv_obj_align(SpiffsWarningBox, NULL, LV_ALIGN_CENTER, 0 ,0);
    
    
    
    //Add button for reboot
    reboot_btn = lv_btn_create( utilities_tile, NULL);
    lv_obj_set_size(reboot_btn, 80, 40);
    lv_obj_set_event_cb( reboot_btn, reboot_utilities_event_cb );
    lv_obj_align( reboot_btn, NULL, LV_ALIGN_CENTER, -80, 100);
    lv_obj_t *reboot_btn_label = lv_label_create( reboot_btn, NULL );
    lv_label_set_text( reboot_btn_label, "Reboot");

    // Add button for poweroff, Equivalent to holding the power button till the hard poweroff state,
    // 300uA power consumption!
    poweroff_btn = lv_btn_create( utilities_tile, NULL);
    lv_obj_set_size(poweroff_btn, 80, 40);
    lv_obj_set_event_cb( poweroff_btn, poweroff_utilities_event_cb );
    lv_obj_align( poweroff_btn, NULL, LV_ALIGN_CENTER, 0, 100);
    lv_obj_t *poweroff_btn_label = lv_label_create( poweroff_btn, NULL );
    lv_label_set_text( poweroff_btn_label, "Poweroff");
    
    lv_obj_t *last_reboot_label = lv_label_create( utilities_tile, NULL);
    lv_obj_add_style( last_reboot_label, LV_OBJ_PART_MAIN, &utilities_style  );
    lv_label_set_text( last_reboot_label, "Last Reboot Reason:");
    lv_obj_align( last_reboot_label, NULL, LV_ALIGN_IN_LEFT_MID, 0, 45 );
    
    lv_obj_t *last_reason_label = lv_label_create( utilities_tile, NULL);
    lv_obj_add_style( last_reason_label, LV_OBJ_PART_MAIN, &utilities_style  );
    lv_label_set_text( last_reason_label, "");
   
    
    esp_reset_reason_t why = esp_reset_reason();
    
    switch (why){
      case (ESP_RST_UNKNOWN):
                                        lv_label_set_text( last_reason_label, "Unknown");
                                        break;
      case (ESP_RST_POWERON):
                                        lv_label_set_text( last_reason_label, "Power On");
                                        break;
      case (ESP_RST_EXT):
                                        lv_label_set_text( last_reason_label, "External Pin");
                                        break;
      case (ESP_RST_SW):
                                        lv_label_set_text( last_reason_label, "Software Reset");
                                        break;
      case (ESP_RST_PANIC):
                                        lv_label_set_text( last_reason_label, "Exception/Panic");
                                        break;
      case (ESP_RST_INT_WDT):
                                        lv_label_set_text( last_reason_label, "Interrupt Watchdog");
                                        break;
      case (ESP_RST_TASK_WDT):
                                        lv_label_set_text( last_reason_label, "Task Watchdog");
                                        break;
      case (ESP_RST_WDT):
                                        lv_label_set_text( last_reason_label, "Other Watchdogs");
                                        break;
      case (ESP_RST_DEEPSLEEP):
                                        lv_label_set_text( last_reason_label, "Exit Deep Sleep");
                                        break;
      case (ESP_RST_BROWNOUT):
                                        lv_label_set_text( last_reason_label, "Brownout");
                                        break;
      case (ESP_RST_SDIO):
                                        lv_label_set_text( last_reason_label, "Reset Over SDIO");
                                        break;
      default:
                                        lv_label_set_text( last_reason_label, "No Reason Returned");
                                        break;
    }
    lv_obj_align( last_reason_label, NULL, LV_ALIGN_IN_LEFT_MID, 0, 60 );
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

static void SpiffsWarningBox_event_handler( lv_obj_t * obj, lv_event_t event ){}

static void format_SPIFFS_utilities_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
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