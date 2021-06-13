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
#include "esp_task_wdt.h"
#include <TTGO.h>

#include "gui/gui.h"

#include "hardware/hardware.h"
#include "hardware/powermgm.h"

#include "app/weather/weather.h"
#include "app/stopwatch/stopwatch_app.h"
#include "app/alarm_clock/alarm_clock.h"
//#include "app/sailing/sailing.h"
#include "app/osmand/osmand_app.h"
#include "app/tiltmouse/tiltmouse_app.h"
#include "app/IRController/IRController.h"
//#include "app/fx_rates/fx_rates.h"
#include "app/activity/activity.h"
#include "app/calc/calc_app.h"
#include "app/calendar/calendar.h"
#include "app/powermeter/powermeter_app.h"
#include "app/FindPhone/FindPhone.h"
//#include "app/gps_status/gps_status.h"
#include "app/osmmap/osmmap_app.h"
#include "app/mqtt_control/mqtt_control_app.h"
#include "app/mqtt_player/mqtt_player_app.h"
#include "app/NetTools/NetTools.h"
#include "app/ping/ping_app.h"
#include "app/wireless/wireless_app.h"
#include "app/wifimon/wifimon_app.h"
#include "app/games/ttt/ttt_game.h"
#include "app/games/pong/pong_game.h"

void setup() {
    /**
     * hardware setup
     * 
     * /hardware/hardware.cpp
     */
    Serial.begin(115200);
    log_i("starting t-watch %s, version: " __FIRMWARE__ " core: %d", WATCH_VERSION_NAME, xPortGetCoreID() );
    log_i("Configure watchdog to 30s: %d", esp_task_wdt_init( 30, true ) );
    hardware_setup();
    /**
     * gui setup
     * 
     * /gui/gui.cpp
     */
    gui_setup();
    /**
     * add apps here!!!
     * 
     * inlude your header file
     * and call your app setup
     */
    osmmap_app_setup();
    weather_app_setup();
    stopwatch_app_setup();
    alarm_clock_setup();
    activity_app_setup();
    calendar_app_setup();
    calc_app_setup();
    //gps_status_setup();
    //sailing_setup();
    osmand_app_setup();
    tiltmouse_app_setup();
    IRController_setup();
    //fxrates_app_setup();
    powermeter_app_setup();
	FindPhone_setup();
    mqtt_control_app_setup();
    mqtt_player_app_setup();
    NetTools_setup();
    ping_app_setup();
    wireless_app_setup();
    wifimon_app_setup();
    tic_tac_toe_game_setup();
    pong_game_setup();
    /**
     * post hardware setup
     * 
     * /hardware/hardware.cpp
     */
    hardware_post_setup();
}

void loop() {
    powermgm_loop();
}
