/*
Copyright (c) 2019 lewis he
This is just a demonstration. Most of the functions are not implemented.
The main implementation is low-power standby. 
The off-screen standby (not deep sleep) current is about 4mA.
Select standard motherboard and standard backplane for testing.
Created by Lewis he on October 10, 2019.
*/
#include "config.h"
#include <Arduino.h>

#include "gui/gui.h"
#include "gui/splashscreen.h"
#include "gui/screenshot.h"

#include "hardware/display.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"

TTGOClass *ttgo = TTGOClass::getWatch();

void setup()
{
    motor_setup();
    Serial.begin(115200);
    Serial.printf("starting t-watch V1\r\n");
    ttgo->begin();
    ttgo->lvgl_begin();

    SPIFFS.begin();
    display_setup( ttgo );

    screenshot_setup();

    splash_screen_stage_one( ttgo );
    splash_screen_stage_update( "init serial", 10 );

    splash_screen_stage_update( "init spiff", 20 );
    if ( !SPIFFS.begin() ) {        
        splash_screen_stage_update( "format spiff", 30 );
        SPIFFS.format();
    }

    splash_screen_stage_update( "init rtc", 50 );
    ttgo->rtc->syncToSystem();
    splash_screen_stage_update( "init powermgm", 100 );
    powermgm_setup( ttgo );
    splash_screen_stage_update( "init gui", 100 );
    splash_screen_stage_finish( ttgo );

    gui_setup(); 
    lv_task_handler();

    for( int bl = 0 ; bl < display_get_brightness() ; bl++ ) {
        ttgo->bl->adjust( bl );
        delay(1);
    } 
}

void loop()
{
    gui_loop( ttgo );
    powermgm_loop( ttgo );
}
