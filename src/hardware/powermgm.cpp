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
#ifndef NATIVE_64BIT
    #ifndef NO_BLUETOOTH
        #include "blectl.h"
    #endif
#endif

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
    #include <Ticker.h>

    Ticker *powermgm_tickTicker = nullptr;
    EventGroupHandle_t powermgm_status = NULL;
    TaskHandle_t _powermgmTask;
    portMUX_TYPE DRAM_ATTR powermgmMux = portMUX_INITIALIZER_UNLOCKED;
    esp_pm_config_esp32_t pm_config;
#endif

callback_t *powermgm_callback = NULL;
callback_t *powermgm_loop_callback = NULL;
static uint32_t lighsleep = 0;

#ifndef NATIVE_64BIT
#include "esp_sleep.h"
#ifdef PARKINSENSE_BMA_FIFO
#include "hardware/bma_fifo.h"
#endif
#include "esp_timer.h"
#include "driver/gpio.h"

static uint64_t powermgm_total_awake_ms = 0;
static uint64_t powermgm_total_sleep_ms = 0;
static uint32_t powermgm_awake_epoch_ms = 0;
static uint32_t powermgm_light_sleep_count = 0;
static uint32_t powermgm_standby_blocked_count = 0;
static uint32_t powermgm_wake_timer_count = 0;
static uint32_t powermgm_wake_gpio_count = 0;
static uint32_t powermgm_wake_other_count = 0;
static char powermgm_last_wake_name[12] = "NONE";
static char powermgm_last_gpio_wake[24] = "";
static uint32_t powermgm_last_sleep_ms = 0;

typedef struct {
    gpio_num_t pin;
    const char *tag;
    gpio_int_type_t trigger;
} powermgm_gpio_wake_candidate_t;

static bool powermgm_gpio_level_matches_trigger( int level, gpio_int_type_t trigger ) {
    switch ( trigger ) {
        case GPIO_INTR_LOW_LEVEL:  return level == 0;
        case GPIO_INTR_HIGH_LEVEL: return level == 1;
        default:                   return false;
    }
}

static uint64_t powermgm_get_gpio_wake_mask( void ) {
#if defined( ESP_IDF_VERSION ) && ( ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL( 4, 4, 0 ) )
    return esp_sleep_get_gpio_wakeup_status();
#else
    uint64_t mask = 0;
    const powermgm_gpio_wake_candidate_t candidates[] = {
#if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        { (gpio_num_t)35, "AXP", GPIO_INTR_LOW_LEVEL },
        { (gpio_num_t)37, "RTC", GPIO_INTR_LOW_LEVEL },
        { (gpio_num_t)39, "BMA", GPIO_INTR_HIGH_LEVEL },
#endif
    };
    for ( size_t i = 0; i < sizeof( candidates ) / sizeof( candidates[0] ); i++ ) {
        const int level = gpio_get_level( candidates[i].pin );
        if ( powermgm_gpio_level_matches_trigger( level, candidates[i].trigger ) )
            mask |= 1ULL << candidates[i].pin;
    }
    return mask;
#endif
}

static void powermgm_stats_note_gpio_wake( void ) {
    const uint64_t mask = powermgm_get_gpio_wake_mask();
    powermgm_last_gpio_wake[0] = '\0';

#if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    const powermgm_gpio_wake_candidate_t candidates[] = {
        { (gpio_num_t)35, "AXP", GPIO_INTR_LOW_LEVEL },
        { (gpio_num_t)37, "RTC", GPIO_INTR_LOW_LEVEL },
        { (gpio_num_t)39, "BMA", GPIO_INTR_HIGH_LEVEL },
    };
    char *out = powermgm_last_gpio_wake;
    size_t out_left = sizeof( powermgm_last_gpio_wake );
    for ( size_t i = 0; i < sizeof( candidates ) / sizeof( candidates[0] ); i++ ) {
        if ( !( mask & ( 1ULL << candidates[i].pin ) ) )
            continue;
        const int n = snprintf(
            out,
            out_left,
            "%s%d:%s",
            out == powermgm_last_gpio_wake ? "" : ",",
            (int)candidates[i].pin,
            candidates[i].tag
        );
        if ( n <= 0 || (size_t)n >= out_left )
            break;
        out += n;
        out_left -= (size_t)n;
    }
    log_i(
        "[PWR] GPIO wake mask=0x%llX pins=%s | L35=%d L37=%d L39=%d",
        (unsigned long long)mask,
        powermgm_last_gpio_wake[0] ? powermgm_last_gpio_wake : "?",
        gpio_get_level( (gpio_num_t)35 ),
        gpio_get_level( (gpio_num_t)37 ),
        gpio_get_level( (gpio_num_t)39 )
    );
#else
    if ( mask ) {
        snprintf(
            powermgm_last_gpio_wake,
            sizeof( powermgm_last_gpio_wake ),
            "0x%llX",
            (unsigned long long)mask
        );
    }
    log_i( "[PWR] GPIO wake mask=0x%llX", (unsigned long long)mask );
#endif
}

static const char* powermgm_wake_cause_name( esp_sleep_wakeup_cause_t cause ) {
    switch ( cause ) {
        case ESP_SLEEP_WAKEUP_TIMER:    return "TIMER";
        case ESP_SLEEP_WAKEUP_GPIO:     return "GPIO";
        case ESP_SLEEP_WAKEUP_UART:     return "UART";
        case ESP_SLEEP_WAKEUP_TOUCHPAD: return "TOUCH";
        case ESP_SLEEP_WAKEUP_EXT0:     return "EXT0";
        case ESP_SLEEP_WAKEUP_EXT1:     return "EXT1";
        case ESP_SLEEP_WAKEUP_ULP:      return "ULP";
        default:                        return "OTHER";
    }
}

static void powermgm_stats_mark_awake_start( void ) {
    powermgm_awake_epoch_ms = millis();
}

static void powermgm_stats_flush_awake( void ) {
    if ( !powermgm_awake_epoch_ms )
        return;
    powermgm_total_awake_ms += millis() - powermgm_awake_epoch_ms;
    powermgm_awake_epoch_ms = 0;
}

static void powermgm_stats_add_sleep_us( int64_t sleep_us ) {
    if ( sleep_us <= 0 )
        return;
    const uint32_t slept_ms = (uint32_t)( sleep_us / 1000 );
    const uint32_t add_ms = slept_ms > 0 ? slept_ms : 1;
    powermgm_last_sleep_ms = add_ms;
    powermgm_total_sleep_ms += add_ms;
}

static void powermgm_stats_record_wake( esp_sleep_wakeup_cause_t cause ) {
    const char* name = powermgm_wake_cause_name( cause );
    strncpy( powermgm_last_wake_name, name, sizeof( powermgm_last_wake_name ) - 1 );
    powermgm_last_wake_name[ sizeof( powermgm_last_wake_name ) - 1 ] = '\0';
    powermgm_last_gpio_wake[0] = '\0';
    switch ( cause ) {
        case ESP_SLEEP_WAKEUP_TIMER:    powermgm_wake_timer_count++; break;
        case ESP_SLEEP_WAKEUP_GPIO:
            powermgm_wake_gpio_count++;
            powermgm_stats_note_gpio_wake();
            break;
        default:                        powermgm_wake_other_count++; break;
    }
    log_i(
        "[PWR] wake %s (%d) last=%u ms | sleep_total=%llu ms awake_total=%llu ms T=%u G=%u O=%u z=%u%s%s",
        name,
        (int)cause,
        (unsigned)powermgm_last_sleep_ms,
        (unsigned long long)powermgm_total_sleep_ms,
        (unsigned long long)powermgm_total_awake_ms,
        (unsigned)powermgm_wake_timer_count,
        (unsigned)powermgm_wake_gpio_count,
        (unsigned)powermgm_wake_other_count,
        (unsigned)powermgm_light_sleep_count,
        powermgm_last_gpio_wake[0] ? " gpio=" : "",
        powermgm_last_gpio_wake[0] ? powermgm_last_gpio_wake : ""
    );
}
#endif


bool powermgm_button_event_cb( EventBits_t event, void *arg );
bool powermgm_send_event_cb( EventBits_t event );
bool powermgm_send_loop_event_cb( EventBits_t event );

void powermgm_setup( void ) {

#ifdef NATIVE_64BIT
    powermgm_status = 0;
#else
    _powermgmTask = xTaskGetCurrentTaskHandle();
    powermgm_status = xEventGroupCreate();

    powermgm_tickTicker = new Ticker();
    #if defined( LILYGO_WATCH_2021 ) || defined( WT32_SC01 )
        powermgm_tickTicker->attach_ms( 100, []() {
            powermgm_resume_from_ISR();
        });
    #else
        powermgm_tickTicker->attach_ms( 1000, []() {
            powermgm_resume_from_ISR();
        });
    #endif
#endif
    /*
     * register powerbutton event
     */
    button_register_cb( BUTTON_PWR, powermgm_button_event_cb, "powermgm pwr button event");
#ifndef NATIVE_64BIT
    powermgm_stats_mark_awake_start();
#endif
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
    static bool standby = true;
    #ifdef NATIVE_64BIT
        /**
         * delay loop fpr 5ms
         */
        SDL_Delay(5);
    #else
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
                pm_config.light_sleep_enable = lighsleep ? false : true ;
                ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
                log_i("custom arduino-esp32 framework detected, enable PM/DFS support, %d/%dMHz %s light sleep (%d)", pm_config.max_freq_mhz, pm_config.min_freq_mhz, lighsleep ? "without" : "with", lighsleep );
            #else
                #ifndef NATIVE_64BIT
                    setCpuFrequencyMhz(80);
                    log_d("CPU speed = 80MHz");
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
                pm_config.light_sleep_enable = lighsleep ? false : true ;
                ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
                log_i("custom arduino-esp32 framework detected, enable PM/DFS support, %d/%dMHz %s light sleep (%d)", pm_config.max_freq_mhz, pm_config.min_freq_mhz, lighsleep ? "without" : "with", lighsleep );
            #else
                #ifndef NATIVE_64BIT
                    setCpuFrequencyMhz(240);
                    log_d("CPU speed = 240MHz");
                #endif
            #endif
        }
        #ifndef NATIVE_64BIT
            log_d("Free heap: %d", ESP.getFreeHeap());
            log_d("Free PSRAM heap: %d", ESP.getFreePsram());
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
        standby = powermgm_send_event_cb( POWERMGM_STANDBY );
        log_i("[PWR_BLE] powermgm: STANDBY callbacks done -> allow_esp_light_sleep=%d (if 0, blocked path; BLE policy unchanged by firmware teardown)", (int)standby );
        // powermgm_disable_interrupts();
        /*
         * print some memory stats
         */
        #ifndef NATIVE_64BIT
            log_d("Free heap: %d", ESP.getFreeHeap());
            log_d("Free PSRAM heap: %d", ESP.getFreePsram());
            log_i("%s uptime: %d", HARDWARE_NAME, millis() / 1000 );
        #endif

        if ( standby ) {
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
                #ifndef NO_BLUETOOTH
                blectl_before_light_sleep();
                #endif
                setCpuFrequencyMhz( 80 );
                log_i("[PWR_BLE] powermgm: esp_light_sleep_start() next (modem/BLE may still cause wakeups)");
                log_d("CPU speed = 80MHz, start light sleep");
                powermgm_stats_flush_awake();
                powermgm_light_sleep_count++;
                {
                    const int64_t sleep_started_us = esp_timer_get_time();
                /*
                * from here, the consumption is round about 2.5mA
                * total standby time is 152h (6days) without use?
                */
                esp_light_sleep_start();
                    powermgm_stats_add_sleep_us( esp_timer_get_time() - sleep_started_us );
                }
#ifdef PARKINSENSE_BMA_FIFO
                if ( bma_fifo_is_active() ) {
                    bma_fifo_service();
                }
#endif
                #ifndef NO_BLUETOOTH
                blectl_after_light_sleep();
                #endif
                /**
                 * check wakeup source
                 */
                {
                    const esp_sleep_wakeup_cause_t wake_cause = esp_sleep_get_wakeup_cause();
                    powermgm_stats_record_wake( wake_cause );
                    powermgm_stats_mark_awake_start();
                    switch ( wake_cause ) {
                        case ESP_SLEEP_WAKEUP_TIMER:
                            log_d("timer wakeup");
                            powermgm_set_event( POWERMGM_SILENCE_WAKEUP_REQUEST );
                            esp_sleep_disable_wakeup_source( ESP_SLEEP_WAKEUP_TIMER );
                            log_d("disable wakeup timer");
                            break;
                        default:
                            log_i(
                                "[PWR_BLE] powermgm: light sleep wake cause=%d (%s)",
                                (int)wake_cause,
                                powermgm_wake_cause_name( wake_cause )
                            );
                            break;
                    }
                }
                /**
                 * after wakeup set to 240MHz
                 */
                #if CONFIG_PM_ENABLE
                    pm_config.max_freq_mhz = 240;
                    pm_config.min_freq_mhz = 80;
                    pm_config.light_sleep_enable = lighsleep ? false : true ;
                    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
                    log_i("custom arduino-esp32 framework detected, enable PM/DFS support, %d/%dMHz %s light sleep (%d)", pm_config.max_freq_mhz, pm_config.min_freq_mhz, lighsleep ? "without" : "with", lighsleep );
                #else
                    #ifndef NATIVE_64BIT
                        setCpuFrequencyMhz(240);
                        log_d("CPU speed = 240MHz");
                    #endif
                #endif
            #endif
        }
        else {
            powermgm_standby_blocked_count++;
            log_w("go standby blocked");
            log_i("[PWR_BLE] powermgm: standby blocked -> 80MHz / esp_pm light sleep path (~20mA with BLE if CONFIG_PM_ENABLE)" );
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
                pm_config.light_sleep_enable = lighsleep ? false : true ;
                ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
                log_d("custom arduino-esp32 framework detected, enable PM/DFS support, %d/%dMHz %s light sleep (%d)", pm_config.max_freq_mhz, pm_config.min_freq_mhz, lighsleep ? "without" : "with", lighsleep );
            #else
                /*
                 * from here, the consumption is round about 28mA with ble
                 * total standby time is 10h with a 350mAh battery
                 */
                #ifndef NATIVE_64BIT
                    setCpuFrequencyMhz(80);
                    log_d("CPU speed = 80MHz");
                #endif
            #endif
        }
    }
    /*
     * send loop event depending on powermem state
     */
    if ( powermgm_get_event( POWERMGM_STANDBY ) ) {
        /*
         * suspend powermgm Task
         */
        if ( !standby )
            powermgm_suspend();
        /**
         * call powermgm loop standby cb
         */
        powermgm_send_loop_event_cb( POWERMGM_STANDBY );
    }
    else if ( powermgm_get_event( POWERMGM_WAKEUP ) ) {
        /**
         * call powermgm loop wakeup cb
         */
        powermgm_send_loop_event_cb( POWERMGM_WAKEUP );
    }
    else if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP ) ) {
        /**
         * call powermgm loop silence wakeup cb
         */
        powermgm_send_loop_event_cb( POWERMGM_SILENCE_WAKEUP );
    }
}

void powermgm_shutdown( void ) {
    powermgm_send_event_cb( POWERMGM_SHUTDOWN );
}

void powermgm_reset( void ) {
    powermgm_send_event_cb( POWERMGM_RESET );
}

void powermgm_suspend( void ) {
    #ifdef NATIVE_64BIT
    #else
        vTaskSuspend( _powermgmTask );
    #endif
}

void powermgm_resume_from_ISR( void ) {
    #ifdef NATIVE_64BIT
    #else
        xTaskResumeFromISR( _powermgmTask );
    #endif
}

void powermgm_resume( void ) {
    #ifdef NATIVE_64BIT
    #else
        vTaskResume( _powermgmTask );
    #endif
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
        pm_config.light_sleep_enable = true;
        ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    #else
        #ifndef NATIVE_64BIT
            setCpuFrequencyMhz(240);
        #endif
    #endif
}

void powermgm_set_lightsleep( bool enable ) {
    if( enable ) {
        if( lighsleep > 0 )
            lighsleep--;
    }
    else
        lighsleep++;
}

void powermgm_set_resume_interval( int32_t interval ) {
    #ifdef NATIVE_64BIT
    #else
        powermgm_tickTicker->attach_ms( interval, []() {
            powermgm_resume_from_ISR();
        });
    #endif
}

bool powermgm_get_lightsleep( void ) {
    return( lighsleep ? true : false );
}

void powermgm_set_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        powermgm_status |= bits;
    #else
        portENTER_CRITICAL(&powermgmMux);
        xEventGroupSetBits( powermgm_status, bits );
        portEXIT_CRITICAL(&powermgmMux);
        powermgm_resume_from_ISR();
    #endif
}

void powermgm_clear_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        powermgm_status &= ~bits;
    #else
        portENTER_CRITICAL(&powermgmMux);
        xEventGroupClearBits( powermgm_status, bits );
        portEXIT_CRITICAL(&powermgmMux);
        powermgm_resume_from_ISR();
    #endif
}

EventBits_t powermgm_get_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        EventBits_t temp = powermgm_status & bits;
    #else
        portENTER_CRITICAL(&powermgmMux);
        EventBits_t temp = xEventGroupGetBits( powermgm_status ) & bits;
        portEXIT_CRITICAL(&powermgmMux);
        powermgm_resume_from_ISR();
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

bool powermgm_register_loop_cb_with_prio( EventBits_t event, CALLBACK_FUNC callback_func, const char *id, callback_prio_t prio ) {
    if ( powermgm_loop_callback == NULL ) {
        powermgm_loop_callback = callback_init( "powermgm loop" );
        if ( powermgm_loop_callback == NULL ) {
            log_e("powermgm loop callback alloc failed");
            while(true);
        }
    }    
    return( callback_register_with_prio( powermgm_loop_callback, event, callback_func, id, prio ) );
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

#ifndef NATIVE_64BIT
uint64_t powermgm_get_awake_ms( void ) {
    uint64_t total = powermgm_total_awake_ms;
    if ( powermgm_awake_epoch_ms )
        total += millis() - powermgm_awake_epoch_ms;
    return total;
}

uint64_t powermgm_get_sleep_ms( void ) {
    return powermgm_total_sleep_ms;
}

unsigned powermgm_get_awake_percent( void ) {
    const uint64_t awake = powermgm_get_awake_ms();
    const uint64_t sleep = powermgm_total_sleep_ms;
    const uint64_t sum = awake + sleep;
    if ( !sum )
        return 0;
    return (unsigned)( ( awake * 100ULL ) / sum );
}

const char* powermgm_get_last_wake_name( void ) {
    return powermgm_last_wake_name;
}

const char* powermgm_get_last_gpio_wake_pins( void ) {
    return powermgm_last_gpio_wake;
}

uint32_t powermgm_get_wake_timer_count( void ) {
    return powermgm_wake_timer_count;
}

uint32_t powermgm_get_wake_gpio_count( void ) {
    return powermgm_wake_gpio_count;
}

uint32_t powermgm_get_wake_other_count( void ) {
    return powermgm_wake_other_count;
}

uint32_t powermgm_get_light_sleep_count( void ) {
    return powermgm_light_sleep_count;
}

uint32_t powermgm_get_standby_blocked_count( void ) {
    return powermgm_standby_blocked_count;
}

uint32_t powermgm_get_last_sleep_ms( void ) {
    return powermgm_last_sleep_ms;
}
#else
uint64_t powermgm_get_awake_ms( void ) { return 0; }
uint64_t powermgm_get_sleep_ms( void ) { return 0; }
unsigned powermgm_get_awake_percent( void ) { return 0; }
const char* powermgm_get_last_wake_name( void ) { return "N/A"; }
const char* powermgm_get_last_gpio_wake_pins( void ) { return ""; }
uint32_t powermgm_get_wake_timer_count( void ) { return 0; }
uint32_t powermgm_get_wake_gpio_count( void ) { return 0; }
uint32_t powermgm_get_wake_other_count( void ) { return 0; }
uint32_t powermgm_get_light_sleep_count( void ) { return 0; }
uint32_t powermgm_get_standby_blocked_count( void ) { return 0; }
uint32_t powermgm_get_last_sleep_ms( void ) { return 0; }
#endif

