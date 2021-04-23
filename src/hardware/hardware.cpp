#include "config.h"
#include <Arduino.h>
#include "esp_bt.h"
#include "esp_task_wdt.h"
#include <TTGO.h>

#include "gui/splashscreen.h"
#include "gui/screenshot.h"
#include "gui/mainbar/mainbar.h"
#include "gui/screenshot.h"

#include "hardware/blectl.h"
#include "hardware/bma.h"
#include "hardware/callback.h"
#include "hardware/display.h"
#include "hardware/gpsctl.h"
#include "hardware/motor.h"
#include "hardware/pmu.h"
#include "hardware/powermgm.h"
#include "hardware/rtcctl.h"
#include "hardware/sound.h"
#include "hardware/timesync.h"
#include "hardware/touch.h"
#include "hardware/wifictl.h"

#include "utils/fakegps.h"

void hardware_setup( void ) {
    /**
     * pre hardware/powermgm setup
     */
    powermgm_setup();

    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->lvgl_begin();
    SPIFFS.begin();
    motor_setup();
    display_setup();
    screenshot_setup();

    splash_screen_stage_one();
    splash_screen_stage_update( "init serial", 10 );
    splash_screen_stage_update( "init spiff", 20 );
    if ( !SPIFFS.begin() ) {
        splash_screen_stage_update( "format spiff", 30 );
        SPIFFS.format();
        splash_screen_stage_update( "format spiff done", 40 );
        delay(500);
        bool remount_attempt = SPIFFS.begin();
        if (!remount_attempt){
            splash_screen_stage_update( "Err: SPIFF Failed", 0 );
            delay(3000);
            ESP.restart();
        }
    }

    splash_screen_stage_update( "init hardware", 60 );    

    pmu_setup();
    bma_setup();
    wifictl_setup();
    touch_setup();
    timesync_setup();
    rtcctl_setup();
    blectl_read_config();
    sound_read_config();
    fakegps_setup();
    
    splash_screen_stage_update( "init gui", 80 );
    splash_screen_stage_finish();
}

void hardware_post_setup( void ) {
    if ( wifictl_get_autoon() && ( pmu_is_charging() || pmu_is_vbus_plug() || ( pmu_get_battery_voltage() > 3400) ) ) {
        wifictl_on();
    }
    sound_setup();
    gpsctl_setup();
    powermgm_set_event( POWERMGM_WAKEUP );
    blectl_setup();

    display_set_brightness( display_get_brightness() );

    delay(500);

    log_i("Total heap: %d", ESP.getHeapSize());
    log_i("Free heap: %d", ESP.getFreeHeap());
    log_i("Total PSRAM: %d", ESP.getPsramSize());
    log_i("Free PSRAM: %d", ESP.getFreePsram());

    disableCore0WDT();
}
