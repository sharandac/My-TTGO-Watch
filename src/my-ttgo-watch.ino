/****************************************************************************
              my-ttgo-watch.ino

    Tu May 22 21:23:51 2020
    Copyright  2020  Dirk Brosswick
 *  Email: dirk.brosswick@googlemail.com
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
#include <Arduino.h>
#include "esp_bt.h"

#include "gui/gui.h"
#include "gui/splashscreen.h"
#include "gui/screenshot.h"

#include "hardware/display.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"
#include "hardware/wifictl.h"
#include "hardware/blectl.h"
#include "hardware/pmu.h"
#include "hardware/timesync.h"

//#include "app/weather/weather.h"
//#include "app/stopwatch/stopwatch_app.h"
//#include "app/crypto_ticker/crypto_ticker.h"
//#include "app/example_app/example_app.h"
//#include "app/osmand/osmand_app.h"

TTGOClass *ttgo = TTGOClass::getWatch();

unsigned long previousMillis = 0;
const long interval = 5; // interval at which for loop (milliseconds)

void setup()
{
    Serial.begin(115200);
    Serial.printf("starting t-watch V1, version: " __FIRMWARE__ "\r\n");

    ttgo->begin();
    ttgo->lvgl_begin();

    SPIFFS.begin();
    motor_setup();

    // force to store all new heap allocations in psram to get more internal ram
    heap_caps_malloc_extmem_enable( 1 );
    display_setup();
    screenshot_setup();
    splash_screen_stage_one();
    splash_screen_stage_update( "init serial", 10 );

    splash_screen_stage_update( "init spiff", 20 );
    if ( !SPIFFS.begin() ) {
        splash_screen_stage_update( "format spiff", 30 );
        SPIFFS.format();
    }

    splash_screen_stage_update( "init rtc", 50 );
    timesyncToSystem();
    splash_screen_stage_update( "init powermgm", 60 );
    powermgm_setup();
    splash_screen_stage_update( "init gui", 80 );
    splash_screen_stage_finish();
    gui_setup();
    /*
     * add apps and widgets here!!!
     */

    // TODO: reactivate when double check that wifi is working
    // weather_app_setup();
    // stopwatch_app_setup();
    // crypto_ticker_setup();
    // example_app_setup();
    // osmand_app_setup();
    /*
     *
     */
    if ( wifictl_get_autoon() && ( pmu_is_charging() || pmu_is_vbus_plug() ) )
         wifictl_on();

    // enable to store data in normal heap
    heap_caps_malloc_extmem_enable( 16*1024 );
    blectl_setup();

    display_set_brightness( display_get_brightness() );

    delay(500);

    Serial.printf("Total heap: %d\r\n", ESP.getHeapSize());
    Serial.printf("Free heap: %d\r\n", ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\r\n", ESP.getFreePsram());

    disableCore0WDT();
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    gui_loop();
    powermgm_loop();
  }
}
