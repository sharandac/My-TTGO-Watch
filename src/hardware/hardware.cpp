#include "config.h"

#include "hardware.h"
#include "button.h"
#include "powermgm.h"
#include "framebuffer.h"
#include "touch.h"
#include "motion.h"
#include "display.h"
#include "gpsctl.h"
#include "timesync.h"
#include "sound.h"
#include "motor.h"
#include "pmu.h"
#include "rtcctl.h"
#include "sdcard.h"
#include "wifictl.h"
#include "blectl.h"
#include "callback.h"
#include "sensor.h"
#include "device.h"
#include "compass.h"

#include "utils/fakegps.h"
#include "gui/splashscreen.h"
#include "gui/screenshot.h"

#ifdef NATIVE_64BIT
    #include "lvgl.h"
    #include <unistd.h>
    #define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
    #include <SDL2/SDL.h>
    #include "utils/logging.h"

    /**
     * A task to measure the elapsed time for LittlevGL
     * @param data unused
     * @return never return
     */
    static int tick_thread(void * data)
    {
        (void)data;
        log_i("start lvgl ticker");
        while(1) {
            SDL_Delay(5);   /*Sleep for 5 millisecond*/
            lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
        }
        return 0;
    }
#else
    #include <Arduino.h>
    #include <SPIFFS.h>
    #include <Ticker.h>
    #include <pthread.h>
    #include "esp_pthread.h"
    #include "esp_bt.h"
    #include "esp_task_wdt.h"
    #include "lvgl.h"

    #if defined( M5PAPER )
        #include <M5EPD.h>
    #elif defined( M5CORE2 )
        #include <M5Core2.h>
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <TTGO.h>
    #elif defined( LILYGO_WATCH_2021 )    
        #include <twatch2021_config.h>
        #include <Wire.h>
    #elif defined( WT32_SC01 )
        #include <Wire.h>
    #else
        #error "no hardware init"
    #endif

    Ticker *tickTicker = nullptr;
#endif

void hardware_attach_lvgl_ticker( void ) {
    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
        tickTicker->attach_ms( 5, []() {
            lv_tick_inc(5);
        });
    #endif
}

void hardware_attach_lvgl_ticker_slow( void ) {
    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
        tickTicker->attach_ms(250, []() {
            lv_tick_inc(250);
        });
    #endif
}

void hardware_detach_lvgl_ticker( void ) {
    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
        tickTicker->detach();
    #endif
}

void hardware_setup( void ) {
    #ifdef NATIVE_64BIT
        /**
         * lvgl init
         */
        lv_init();
        // Workaround for sdl2 `-m32` crash
        // https://bugs.launchpad.net/ubuntu/+source/libsdl2/+bug/1775067/comments/7
        #ifndef WIN32
            setenv("DBUS_FATAL_WARNINGS", "0", 1);
        #endif
        /* Tick init.
        * You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about how much time were elapsed
        * Create an SDL thread to do this*/
        SDL_CreateThread( tick_thread, "tick", NULL );
    #else
        esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
        cfg.stack_size = ( 8 * 1024 );
        cfg.inherit_cfg = false;
        esp_pthread_set_cfg(&cfg);   

        #if defined( M5PAPER )
            /**
             * lvgl init
             */
            lv_init();
            /**
             * init M5paper hardware
             */
            M5.begin();
        #elif defined( M5CORE2 )
            /**
             * LVGL init
             */
            lv_init();
            /**
             * init M5Core2 hardware
             */
            M5.begin();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            /**
             * lvgl init
             */
            lv_init();
            /**
             * ttgo init
             */
            ttgo->begin();
        #elif defined( LILYGO_WATCH_2021 )
            /**
             * power all devices
             */
            pinMode( PWR_ON, OUTPUT );
            digitalWrite( PWR_ON, HIGH );
            /**
             * reset touch controler
             */
            pinMode( Touch_Res, OUTPUT );
            digitalWrite( Touch_Res, LOW );
            delay(10);
            digitalWrite( Touch_Res, HIGH );
            delay(50);              
            /**
             * lvgl init
             */
            lv_init();
            /**
             * setup wire interface
             */
//            Wire.begin( IICSDA, IICSCL, 1000000 );
            Wire.begin( IICSDA, IICSCL );
            /**
             * scan i2c devices
             */
            for( uint8_t address = 1; address < 127; address++ ) {
                Wire.beginTransmission(address);
                if ( Wire.endTransmission() == 0 )
                    log_i("I2C device at: 0x%02x", address );

            }
        #elif defined( WT32_SC01 )
            /**
             * lvgl init
             */
            lv_init();     
            /**
             * setup wire interface
             */
//            Wire.begin( IICSDA, IICSCL, 1000000 );
            Wire.begin( PIN_SDA, PIN_SCL );
            /**
             * scan i2c devices
             */
            for( uint8_t address = 1; address < 127; address++ ) {
                Wire.beginTransmission(address);
                if ( Wire.endTransmission() == 0 )
                    log_i("I2C device at: 0x%02x", address );
            }
        #endif
        /**
         * init lvgl ticker
         */    
        tickTicker = new Ticker();
        hardware_attach_lvgl_ticker();
        /*
        * setup spiffs
        */
        SPIFFS.begin();
    #endif
    /**
     * driver init
     */
    device_setup();
    sdcard_setup();
    powermgm_setup();
    button_setup();
    motor_setup();
    display_setup();
    screenshot_setup();
    compass_setup();
    /**
     * splashscreen setup
     */
    splash_screen_stage_one();
    /**
     * work on SPIFFS
     */
    #ifdef NATIVE_64BIT
    #else
        if ( !SPIFFS.begin() ) {
            splash_screen_stage_update( "format spiff", 30 );
            log_i("format SPIFFS");
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
    #endif
    splash_screen_stage_update( "init hardware", 60 );  

    pmu_setup();
    bma_setup();
    wifictl_setup();
    touch_setup();
    rtcctl_setup();
    timesync_setup();
    sensor_setup();
    sound_read_config();
    fakegps_setup();
    blectl_read_config();

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

    #ifndef NO_BLUETOOTH
        blectl_setup();
    #endif

    display_set_brightness( display_get_brightness() );

    #ifdef NATIVE_64BIT
    #else
        log_i("Free heap: %d", ESP.getFreeHeap());
        log_i("Free PSRAM heap: %d", ESP.getFreePsram());
    #endif
}