#include "lvgl.h"
#include "gui/gui.h"

#include "hardware/hardware.h"
#include "hardware/powermgm.h"

#include "app/calc/calc_app.h"
#include "app/FindPhone/FindPhone.h"
//#include "app/example_app/example_app.h"
#include "app/gps_status/gps_status.h"
#include "app/kodi_remote/kodi_remote_app.h"
#include "app/osmand/osmand_app.h"
//#include "app/powermeter/powermeter_app.h"
#include "app/osmmap/osmmap_app.h"
#include "app/mail/mail_app.h"
//#include "app/sailing/sailing.h"
#include "app/stopwatch/stopwatch_app.h"
#include "app/wifimon/wifimon_app.h"
#include "app/calendar/calendar.h"
#include "app/weather/weather.h"
#include "app/activity/activity.h"
//#include "app/fx_rates/fx_rates.h"
#include "app/IRController/IRController.h"
#include "app/alarm_clock/alarm_clock.h"
#include "app/mqtt_player/mqtt_player_app.h"
#include "app/mqtt_control/mqtt_control_app.h"
#include "app/printer3d/printer3d_app.h"
#include "app/tiltmouse/tiltmouse_app.h"
#include "app/weather_station/weather_station_app.h"
#include "app/NetTools/NetTools.h"
#include "app/ping/ping_app.h"
#include "app/wireless/wireless_app.h"
#include "app/games/ttt/ttt_game.h"
#include "app/games/pong/pong_game.h"

#if defined( NATIVE_64BIT )
    /**
     * for non arduino
     */                 
    void setup( void );
    void loop( void );

    int main( void ) {
        setup();
        while( 1 ) { loop(); };
        return( 0 );
    }
#endif // NATIVE_64BIT

void setup() {
    /**
     * hardware setup
     */
    hardware_setup();
    /**
     * gui setup
     */
    gui_setup();
    /**
     * apps here
     */
    stopwatch_app_setup();
    alarm_clock_setup();
    activity_app_setup();
    calendar_app_setup();
    mail_app_setup();
    calc_app_setup();
    printer3d_app_setup();
    weather_app_setup();
    weather_station_app_setup();
    IRController_setup();
    //sailing_setup();
    gps_status_setup();
    osmmap_app_setup();
    osmand_app_setup();
    kodi_remote_app_setup();
    mqtt_player_app_setup();
    mqtt_control_app_setup();
    //fxrates_app_setup();
    //powermeter_app_setup();
    FindPhone_setup();
    tiltmouse_app_setup();
    NetTools_setup();
    ping_app_setup();
    wireless_app_setup();
    wifimon_app_setup();
    tic_tac_toe_game_setup();
    pong_game_setup();
    /**
     * post hardware setup
     */
    hardware_post_setup();
}

void loop(){
    powermgm_loop();
}