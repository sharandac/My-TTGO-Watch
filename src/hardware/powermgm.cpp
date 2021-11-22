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
#include <time.h>
#include "powermgm.h"
#include "callback.h"
#include "button.h"

#ifdef NATIVE_64BIT 
    #include <unistd.h>
    #define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
    #include <SDL2/SDL.h>
    #include "utils/io.h"
    #include "utils/logging.h"

    static EventBits_t powermgm_status;
#else
    #include "esp_err.h"
    #include "esp_pm.h"
    #include <Arduino.h>

    EventGroupHandle_t powermgm_status = NULL;
    portMUX_TYPE DRAM_ATTR powermgmMux = portMUX_INITIALIZER_UNLOCKED;
    esp_pm_config_esp32_t pm_config;
#endif

callback_t *powermgm_callback = NULL;
callback_t *powermgm_loop_callback = NULL;

bool powermgm_button_event_cb( EventBits_t event, void *arg );
bool powermgm_send_event_cb( EventBits_t event );
bool powermgm_send_loop_event_cb( EventBits_t event );

void powermgm_setup( void ) {

#ifdef NATIVE_64BIT
    powermgm_status = 0;
#else
    powermgm_status = xEventGroupCreate();
#endif
    /*
     * register powerbutton event
     */
    button_register_cb( BUTTON_PWR, powermgm_button_event_cb, "powermgm pwr button event");
}

bool powermgm_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_PWR:    powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                            powermgm_set_event( POWERMGM_POWER_BUTTON );
                            break;     
    }
    return( true );
}

void powermgm_loop( void ) {
    static bool lighsleep = true;

    #ifdef NATIVE_64BIT
        /**
         * delay loop fpr 5ms
         */
        SDL_Delay(5);
    #endif // NATIVE_64BIT
    /*
     * check if power button was release
     */
    if( powermgm_get_event( POWERMGM_POWER_BUTTON ) ) {
        if ( powermgm_get_event( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP ) ) {
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
        }
        else {
            powermgm_set_event( POWERMGM_STANDBY_REQUEST );
        }
        powermgm_clear_event( POWERMGM_POWER_BUTTON );
    }
    /*
     * when we are in wakeup and get an wakeup request, reset activity timer
     */
    if ( powermgm_get_event( POWERMGM_WAKEUP_REQUEST ) && powermgm_get_event( POWERMGM_WAKEUP ) ) {
        // lv_disp_trig_activity( NULL );
        powermgm_clear_event( POWERMGM_WAKEUP_REQUEST );
    }

    /*
     * handle powermgm request
     */
    if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP_REQUEST | POWERMGM_WAKEUP_REQUEST ) ) {
        /*
         * clear powermgm state
         */
        powermgm_clear_event( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP );

        if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP_REQUEST ) ) {
            log_i("go silence wakeup");
            /*
             * set silence wakeup status/request and send events
             */
            powermgm_clear_event( POWERMGM_SILENCE_WAKEUP_REQUEST );
            powermgm_set_event( POWERMGM_SILENCE_WAKEUP );
            powermgm_send_event_cb( POWERMGM_SILENCE_WAKEUP );
            /*
             * set cpu speed
             * 
             * note:    CONFIG_PM_ENABLE comes from the arduino IDF and is only use when
             *          an custom arduino in platformio.ini is set. Is CONFIG_PM_ENABLE is set, it enabled
             *          extra features like dynamic frequency scaling. Otherwise normal arduino function
             *          will be used.
             */
            #if CONFIG_PM_ENABLE
                pm_config.max_freq_mhz = 160;
                pm_config.min_freq_mhz = 80;
                pm_config.light_sleep_enable = true;
                ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
                log_i("custom arduino-esp32 framework detected, enable PM/DFS support, 160/80MHz with light sleep");
            #else
                #ifndef NATIVE_64BIT
                    setCpuFrequencyMhz(80);
                    log_i("CPU speed = 80MHz");
                #endif
            #endif
        }
        else {
            log_i("go wakeup");
            /**
             * set wakeup status/request and send events
             */
            powermgm_clear_event( POWERMGM_WAKEUP_REQUEST );
            powermgm_set_event( POWERMGM_WAKEUP );
            powermgm_send_event_cb( POWERMGM_WAKEUP );
            /**
             * set cpu speed
             * 
             * note:    CONFIG_PM_ENABLE comes from the arduino IDF and is only use when
             *          an custom arduino in platformio.ini is set. Is CONFIG_PM_ENABLE is set, it enabled
             *          extra features like dynamic frequency scaling. Otherwise normal arduino function
             *          will be used.
             */
            #if CONFIG_PM_ENABLE
                pm_config.max_freq_mhz = 240;
                pm_config.min_freq_mhz = 80;
                pm_config.light_sleep_enable = false;
                ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
                log_i("custom arduino-esp32 framework detected, enable PM/DFS support, 240/80MHz with light sleep");
            #else
                #ifndef NATIVE_64BIT
                    setCpuFrequencyMhz(240);
                    log_i("CPU speed = 240MHz");
                #endif
            #endif
        }
        #ifndef NATIVE_64BIT
            log_i("Free heap: %d", ESP.getFreeHeap());
            log_i("Free PSRAM heap: %d", ESP.getFreePsram());
            log_i("%s uptime: %d", HARDWARE_NAME, millis() / 1000 );
        #endif
    }        
    else if( powermgm_get_event( POWERMGM_STANDBY_REQUEST ) ) {
        /*
         * avoid buzz when standby after silent wake
         */
        if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP ) ) {
            #ifndef NATIVE_64BIT
                delay( 100 );
            #endif
        }
        /*
         * clear powermgm state/request and send standby event
         */
        powermgm_clear_event( POWERMGM_STANDBY_REQUEST );
        powermgm_clear_event( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP );
        powermgm_set_event( POWERMGM_STANDBY );
        /*
         * send POWERMGM_STANDBY to all registered callback functions and
         * check if an standby callback block lightsleep in standby
         */
        lighsleep = powermgm_send_event_cb( POWERMGM_STANDBY );
        /*
         * print some memory stats
         */
        #ifndef NATIVE_64BIT
            log_i("Free heap: %d", ESP.getFreeHeap());
            log_i("Free PSRAM heap: %d", ESP.getFreePsram());
            log_i("%s uptime: %d", HARDWARE_NAME, millis() / 1000 );
        #endif

        if ( lighsleep ) {
            log_i("go standby");
            /*
             * set cpu speed
             * 
             * note:    direct after change the CPU clock, we go to light sleep.
             *          it is no difference in light sleep we have 80Mhz or 10Mhz
             *          CPU clock. Current is the same.
             */
            #ifdef NATIVE_64BIT

            #else
                setCpuFrequencyMhz( 80 );
                log_i("CPU speed = 80MHz, start light sleep");
                /*
                * from here, the consumption is round about 2.5mA
                * total standby time is 152h (6days) without use?
                */
                esp_light_sleep_start();
                /**
                 * check wakeup source
                 */
                switch( esp_sleep_get_wakeup_cause() ) {
                    case ESP_SLEEP_WAKEUP_TIMER:    
                        log_i("timer wakeup");
                        powermgm_set_event( POWERMGM_SILENCE_WAKEUP_REQUEST );
                        break;
                    default:
                        break;
                }
                /**
                 * after wakeup set to 240MHz
                 */
                #if CONFIG_PM_ENABLE
                    pm_config.max_freq_mhz = 240;
                    pm_config.min_freq_mhz = 80;
                    pm_config.light_sleep_enable = false;
                    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
                    log_i("custom arduino-esp32 framework detected, enable PM/DFS support, 240/80MHz with light sleep");
                #else
                    #ifndef NATIVE_64BIT
                        setCpuFrequencyMhz(240);
                        log_i("CPU speed = 240MHz");
                    #endif
                #endif
            #endif
        }
        else {
            log_i("go standby blocked");
            /*
             * set cpu speed
             * 
             * note:    CONFIG_PM_ENABLE comes from the arduino IDF and is only use when
             *          an custom arduino in platformio.ini is set. Is CONFIG_PM_ENABLE is set, it enabled
             *          extra features like dynamic frequency scaling. Otherwise normal arduino function
             *          will be used.
             */
            #if CONFIG_PM_ENABLE
                /*
                 * from here, the consumption is round about 20mA with ble
                 * total standby time is 15h with a 350mAh battery
                 */
                pm_config.max_freq_mhz = 80;
                pm_config.min_freq_mhz = 40;
                pm_config.light_sleep_enable = true;
                ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
                log_i("custom arduino-esp32 framework detected, enable PM/DFS support, 80/40MHz with light sleep");
            #else
                /*
                 * from here, the consumption is round about 28mA with ble
                 * total standby time is 10h with a 350mAh battery
                 */
                #ifndef NATIVE_64BIT
                    setCpuFrequencyMhz(80);
                    log_i("CPU speed = 80MHz");
                #endif
            #endif
        }
    }
    /*
     * send loop event depending on powermem state
     */
    if ( powermgm_get_event( POWERMGM_STANDBY ) ) {
        /*
         * Idle when lightsleep in standby not allowed
         * It make it possible for the IDLE task to trottle
         * down CPU clock or go into light sleep.
         * 
         * note:    When change vTaskDelay to an higher value, please
         *          note that the reaction time to wake up increase.
         */
        #ifdef NATIVE_64BIT
        #else
            if ( !lighsleep )
                vTaskDelay( 250 );
        #endif

        powermgm_send_loop_event_cb( POWERMGM_STANDBY );
    }
    else if ( powermgm_get_event( POWERMGM_WAKEUP ) ) {
        powermgm_send_loop_event_cb( POWERMGM_WAKEUP );
    }
    else if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP ) ) {
        powermgm_send_loop_event_cb( POWERMGM_SILENCE_WAKEUP );
    }
}

void powermgm_shutdown( void ) {
    powermgm_send_event_cb( POWERMGM_SHUTDOWN );
}

void powermgm_reset( void ) {
    powermgm_send_event_cb( POWERMGM_RESET );
}

void powermgm_set_perf_mode( void ) {
    #if CONFIG_PM_ENABLE
        pm_config.max_freq_mhz = 240;
        pm_config.min_freq_mhz = 240;
        pm_config.light_sleep_enable = false;
        ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    #else
        #ifndef NATIVE_64BIT
            setCpuFrequencyMhz(240);
        #endif
    #endif
}

void powermgm_set_normal_mode( void ) {
    #if CONFIG_PM_ENABLE
        pm_config.max_freq_mhz = 240;
        pm_config.min_freq_mhz = 80;
        pm_config.light_sleep_enable = false;
        ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    #else
        #ifndef NATIVE_64BIT
            setCpuFrequencyMhz(240);
        #endif
    #endif
}

void powermgm_set_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        powermgm_status |= bits;
    #else
        portENTER_CRITICAL(&powermgmMux);
        xEventGroupSetBits( powermgm_status, bits );
        portEXIT_CRITICAL(&powermgmMux);
    #endif
}

void powermgm_clear_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        powermgm_status &= ~bits;
    #else
        portENTER_CRITICAL(&powermgmMux);
        xEventGroupClearBits( powermgm_status, bits );
        portEXIT_CRITICAL(&powermgmMux);
    #endif
}

EventBits_t powermgm_get_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        EventBits_t temp = powermgm_status & bits;
    #else
        portENTER_CRITICAL(&powermgmMux);
        EventBits_t temp = xEventGroupGetBits( powermgm_status ) & bits;
        portEXIT_CRITICAL(&powermgmMux);
    #endif
    return( temp );
}

bool powermgm_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( powermgm_callback == NULL ) {
        powermgm_callback = callback_init( "powermgm" );
        if ( powermgm_callback == NULL ) {
            log_e("powermgm callback alloc failed");
            while(true);
        }
    }    
    return( callback_register( powermgm_callback, event, callback_func, id ) );
}

bool powermgm_register_cb_with_prio( EventBits_t event, CALLBACK_FUNC callback_func, const char *id, callback_prio_t prio ) {
    if ( powermgm_callback == NULL ) {
        powermgm_callback = callback_init( "powermgm" );
        if ( powermgm_callback == NULL ) {
            log_e("powermgm callback alloc failed");
            while(true);
        }
    }    
    return( callback_register_with_prio( powermgm_callback, event, callback_func, id, prio ) );
}

bool powermgm_register_loop_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( powermgm_loop_callback == NULL ) {
        powermgm_loop_callback = callback_init( "powermgm loop" );
        if ( powermgm_loop_callback == NULL ) {
            log_e("powermgm loop callback alloc failed");
            while(true);
        }
    }    
    return( callback_register( powermgm_loop_callback, event, callback_func, id ) );
}

bool powermgm_send_event_cb( EventBits_t event ) {
    return( callback_send( powermgm_callback, event, (void*)NULL ) );
}

bool powermgm_send_loop_event_cb( EventBits_t event ) {
    return( callback_send_no_log( powermgm_loop_callback, event, (void*)NULL ) );
}

void powermgm_disable_interrupts( void ) {
    powermgm_send_event_cb( POWERMGM_DISABLE_INTERRUPTS );
}

void powermgm_enable_interrupts( void ) {
    powermgm_send_event_cb( POWERMGM_ENABLE_INTERRUPTS );
}