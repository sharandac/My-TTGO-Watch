/****************************************************************************
 *   Sep 11 10:11:10 2021
 *   Copyright  2021  Dirk Brosswick
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
#include "powermgm.h"
#include "button.h"
#include "callback.h"
#include "utils/alloc.h"

bool button_send_cb( EventBits_t event, void *arg );

#ifdef NATIVE_64BIT
    /*
     * To fix SDL's "undefined reference to WinMain" issue
     */
    #include "utils/logging.h"
    #include <SDL2/SDL.h>
#else
    #include <Arduino.h>
    #if defined( M5PAPER )
        #include <M5EPD.h>
    #elif defined( M5CORE2 )
        #include <M5Core2.h>
        static bool button_active = true;

        HotZone left_btn( 0, 240, 106, 300 );
        HotZone power_btn( 106, 240, 212, 300 );
        HotZone right_btn( 213, 240, 319, 300 );
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include "pmu.h"
        /**
         * special case: on ttgo watch 2020 the button is connected 
         * to the pmu
         */
        bool button_pmu_event_cb( EventBits_t event, void *arg );
        bool button_pmu_event_cb( EventBits_t event, void *arg ) {
            switch( event ) {
                case PMUCTL_SHORT_PRESS:    button_send_cb( BUTTON_PWR, (void *)NULL );
                                            log_d("send BUTTON_PWR event");
                                            break;
                case PMUCTL_LONG_PRESS:     button_send_cb( BUTTON_QUICKBAR, (void *)NULL );
                                            log_d("send BUTTON_QUICKBAR event");
                                            break;
            }
            return( true );
        }
    #elif defined( LILYGO_WATCH_2021 ) 
        #include <twatch2021_config.h>
    #elif defined( WT32_SC01 )
    #else
        #warning "no hardware driver for button"
    #endif
    /**
     * genreal allocated IRQ function
     */
    volatile bool DRAM_ATTR button_irq_flag = false;
    portMUX_TYPE DRAM_ATTR BUTTON_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;
    void IRAM_ATTR button_irq( void );
    /**
     * button IRQ
     */
    void IRAM_ATTR  button_irq( void ) {
        portENTER_CRITICAL_ISR(&BUTTON_IRQ_Mux);
        button_irq_flag = true;
        portEXIT_CRITICAL_ISR(&BUTTON_IRQ_Mux);
        powermgm_resume_from_ISR();
    }
#endif

callback_t *button_callback = NULL;
bool button_powermgm_loop_cb( EventBits_t event, void *arg );
bool button_powermgm_event_cb( EventBits_t event, void *arg );

void button_setup( void ) {
    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
            /**
             * set push button IO
             */
            attachInterrupt( M5EPD_KEY_PUSH_PIN, button_irq, FALLING );
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            /**
             * special case: on ttgo watch 2020 the button is connected 
             * to the pmu
             */
            pmu_register_cb( PMUCTL_SHORT_PRESS | PMUCTL_LONG_PRESS, button_pmu_event_cb, "button pmu event" );
        #elif defined( LILYGO_WATCH_2021 ) 
            pinMode( BTN_1, INPUT_PULLUP );
            pinMode( BTN_2, INPUT );
            pinMode( BTN_3, INPUT );
        #elif defined( WT32_SC01 )

        #endif
    #endif
    /*
     * register all powermem callback functions
     */
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_ENABLE_INTERRUPTS | POWERMGM_DISABLE_INTERRUPTS , button_powermgm_event_cb, "powermgm button" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP , button_powermgm_loop_cb, "powermgm button loop" );
}

bool button_powermgm_loop_cb( EventBits_t event, void *arg ) {
#ifdef NATIVE_64BIT
    static bool left_button = false;
    static bool right_button = false;
    static bool quickbar_button = false;
    static bool exit_button = false;
    static bool media_button = false;
    static bool notify_button = false;
    static bool notify_del_button = false;

    const uint8_t *state = SDL_GetKeyboardState( NULL );

    if ( state[ SDL_SCANCODE_LEFT ] != left_button ) {
        left_button = state[ SDL_SCANCODE_LEFT ];
        if ( left_button ) button_send_cb( BUTTON_LEFT, (void*)NULL );
    }

    if ( state[ SDL_SCANCODE_RIGHT ] != right_button ) {
        right_button = state[ SDL_SCANCODE_RIGHT ];
        if ( right_button ) button_send_cb( BUTTON_RIGHT, (void*)NULL );
    }

    if ( state[ SDL_SCANCODE_RETURN ] != quickbar_button ) {
        quickbar_button = state[ SDL_SCANCODE_RETURN ];
        if ( quickbar_button ) button_send_cb( BUTTON_QUICKBAR, (void*)NULL );
    }

    if ( state[ SDL_SCANCODE_ESCAPE ] != exit_button ) {
        exit_button = state[ SDL_SCANCODE_ESCAPE ];
        if ( exit_button ) button_send_cb( BUTTON_EXIT, (void*)NULL );
    }

    if ( state[ SDL_SCANCODE_P] != notify_button ) {
        notify_button = state[ SDL_SCANCODE_P ];
        if ( notify_button ) button_send_cb( BUTTON_NOTIFY_TEST, (void*)NULL );
    }

    if ( state[ SDL_SCANCODE_D] != notify_del_button ) {
        notify_del_button = state[ SDL_SCANCODE_D ];
        if ( notify_del_button ) button_send_cb( BUTTON_NOTIFY_DEL_TEST, (void*)NULL );
    }

    if ( state[ SDL_SCANCODE_M] != media_button ) {
        media_button = state[ SDL_SCANCODE_M ];
        if ( media_button ) button_send_cb( BUTTON_MEDIA_TEST, (void*)NULL );
    }
#else
    /*
     * handle IRQ event
     */
    portENTER_CRITICAL(&BUTTON_IRQ_Mux);
    bool temp_button_irq_flag = button_irq_flag;
    button_irq_flag = false;
    portEXIT_CRITICAL(&BUTTON_IRQ_Mux);
    
    #ifdef M5PAPER
        static uint64_t push_presstime = millis();
        /**
         * handle push button event
         */
        M5.update();
        if( M5.BtnP.wasPressed() ) {
            log_d("button was pressed");
            push_presstime = millis();
        }
        else if( M5.BtnP.wasReleased() ) {
            log_d("button was release");
            if( !temp_button_irq_flag ) {
                push_presstime = millis() - push_presstime;
                log_d("presstime = %dms", push_presstime );
                if ( push_presstime < 2000 ) {
                    log_d("short press");
                    button_send_cb( BUTTON_PWR, (void *)NULL );
                }
                else if  ( push_presstime < 4500 ) {
                    log_d("long press");
                    button_send_cb( BUTTON_QUICKBAR, (void *)NULL );           
                }
                else {
                    log_d("press state ignore");
                }
            }
            else {
                push_presstime = millis();
            }
        }
        /**
         * handle left/right button event only in wakeup
         */
        if ( event == POWERMGM_WAKEUP ) {
            /**
             * handle left button event
             */
            if ( M5.BtnL.wasPressed() ) {
                log_d("left press");
                button_send_cb( BUTTON_LEFT, (void *)NULL );
            }
            /**
             * handle right button event
             */
            if ( M5.BtnR.wasPressed() ) {
                log_d("right press");
                button_send_cb( BUTTON_RIGHT, (void *)NULL );
            }
        }
    #elif defined( M5CORE2 )
        if( !button_active )
            return( true );

        TouchPoint_t pos = M5.Touch.getPressPoint();

        static bool left_button = left_btn.inHotZone( pos );
        static bool power_button = power_btn.inHotZone( pos );
        static bool right_button = right_btn.inHotZone( pos );
        static uint64_t left_button_time = 0;
        static uint64_t power_button_time = 0;
        static uint64_t right_button_time = 0;
        
        if( left_button != left_btn.inHotZone( pos ) ) {
            left_button = left_btn.inHotZone( pos );
            uint64_t press_time = 0;
            /**
             * start timer
             */
            if ( left_button )
                left_button_time = millis();
            else
                press_time = millis() - left_button_time;
            /**
             * check timer
             */
            if ( press_time != 0 ) {
                if ( press_time < 1000 )
                    button_send_cb( BUTTON_LEFT, (void*)NULL );
                else
                    button_send_cb( BUTTON_EXIT, (void *)NULL );
            }
        } 

        if( power_button != power_btn.inHotZone( pos ) ) {
            power_button = power_btn.inHotZone( pos );
            uint64_t press_time = 0;
            /**
             * start timer
             */
            if ( power_button )
                power_button_time = millis();
            else
                press_time = millis() - power_button_time;
            /**
             * check timer
             */
            if ( press_time != 0 ) {
                if ( press_time < 1000 )
                    button_send_cb( BUTTON_PWR, (void*)NULL );
                else
                    button_send_cb( BUTTON_QUICKBAR, (void *)NULL );
            }
        } 

        if( right_button != right_btn.inHotZone( pos ) ) {
            right_button = right_btn.inHotZone( pos );
            uint64_t press_time = 0;
            /**
             * start timer
             */
            if ( right_button )
                right_button_time = millis();
            else
                press_time = millis() - right_button_time;
            /**
             * check timer
             */
            if ( press_time != 0 ) {
                if ( press_time < 1000 )
                    button_send_cb( BUTTON_RIGHT, (void*)NULL );
                else
                    button_send_cb( BUTTON_SETUP, (void *)NULL );
            }
        } 

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #elif defined( LILYGO_WATCH_2021 ) 
        static bool exit_button = digitalRead( BTN_1 );
        static bool refresh_button = digitalRead( BTN_2 );
        static bool setup_button = digitalRead( BTN_3 );
        static uint64_t exit_button_time = 0;
        static uint64_t refresh_button_time = 0;
        static uint64_t setup_button_time = 0;
        /**
         * BTN_1 logic
         */
        if ( digitalRead( BTN_1 ) != exit_button ) {
            exit_button = digitalRead( BTN_1 );

            uint64_t press_time = 0;
            if ( !exit_button ) {
                exit_button_time = millis();
            }
            else {
                press_time = millis() - exit_button_time;
            }

            if ( press_time != 0 ) {
                /**
                 * special case when we are in standby or silence wakeup
                 */
                if ( powermgm_get_event( POWERMGM_STANDBY ) || powermgm_get_event( POWERMGM_SILENCE_WAKEUP ) ){
                    button_send_cb( BUTTON_PWR, (void *)NULL );
                    powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                }
                else {
                    if( press_time < 250 )
                        button_send_cb( BUTTON_EXIT, (void *)NULL );
                    else if ( press_time < 1000 )
                        button_send_cb( BUTTON_PWR, (void *)NULL );                    
                    else
                        button_send_cb( BUTTON_QUICKBAR, (void *)NULL );
                }
            }
        }
        /**
         * BTN_2 logic
         */
        if ( digitalRead( BTN_2 ) != setup_button ) {
            setup_button = digitalRead( BTN_2 );

            uint64_t press_time = 0;
            if ( setup_button )
                setup_button_time = millis();
            else
                press_time = millis() - setup_button_time;

            if ( press_time != 0 ) {
                /**
                 * special case when we are in standby or silence wakeup
                 */
                if ( powermgm_get_event( POWERMGM_STANDBY ) || powermgm_get_event( POWERMGM_SILENCE_WAKEUP ) ){
                    button_send_cb( BUTTON_PWR, (void *)NULL );
                    powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                }
                else {
                    if ( press_time < 250 )
                        button_send_cb( BUTTON_DOWN, (void*)NULL );
                    else
                        button_send_cb( BUTTON_SETUP, (void *)NULL );
                }
            }
        }
        /**
         * BTN_3 logic
         */
        if ( digitalRead( BTN_3 ) != refresh_button ) {
            refresh_button = digitalRead( BTN_3 );

            uint64_t press_time = 0;
            if ( refresh_button )
                refresh_button_time = millis();
            else
                press_time = millis() - refresh_button_time;

            if ( press_time != 0 ) {
                /**
                 * special case when we are in standby or silence wakeup
                 */
                if ( powermgm_get_event( POWERMGM_STANDBY ) || powermgm_get_event( POWERMGM_SILENCE_WAKEUP ) ){
                    button_send_cb( BUTTON_PWR, (void *)NULL );
                    powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                }
                else {
                    if ( press_time < 250 )
                        button_send_cb( BUTTON_UP, (void*)NULL );
                    else
                        button_send_cb( BUTTON_REFRESH, (void *)NULL );
                }
            }

            if ( refresh_button ) button_send_cb( BUTTON_REFRESH, (void*)NULL );
        }
    #elif defined( WT32_SC01 )
    #endif
    /**
     * prevent "warning: variable 'temp_button_irq_flag' set but not used" in some platform conditions
     */
    temp_button_irq_flag = ~temp_button_irq_flag;
#endif
    return( true );
}

bool button_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = false;

    #ifdef NATIVE_64BIT
        retval = true;
    #else
        #if defined( M5PAPER )
            switch( event ) {
                case POWERMGM_STANDBY:              log_d("button standby");
                                                    /*
                                                    * enable GPIO in lightsleep for wakeup
                                                    */
                                                    gpio_wakeup_enable( (gpio_num_t)M5EPD_KEY_PUSH_PIN, GPIO_INTR_LOW_LEVEL );
                                                    esp_sleep_enable_gpio_wakeup ();
                                                    retval = true;
                                                    break;
                case POWERMGM_WAKEUP:               log_d("button wakeup");
                                                    retval = true;
                                                    break;
                case POWERMGM_SILENCE_WAKEUP:       log_d("button silence wakeup");
                                                    retval = true;
                                                    break;
                case POWERMGM_ENABLE_INTERRUPTS:    log_d("button enable interrupts");
                                                    attachInterrupt( M5EPD_KEY_PUSH_PIN, &button_irq, FALLING );
                                                    retval = true;
                                                    break;
                case POWERMGM_DISABLE_INTERRUPTS:   log_d("button disable interrupts");
                                                    detachInterrupt( M5EPD_KEY_PUSH_PIN );
                                                    retval = true;
                                                    break;
            }
        #elif defined( M5CORE2 )
            switch( event ) {
                case POWERMGM_STANDBY:
                    button_active = false;
                    break;
                case POWERMGM_SILENCE_WAKEUP:
                    button_active = false;
                    break;
                case POWERMGM_WAKEUP:
                    button_active = true;
                    break;
            }
            retval = true;
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            retval = true;
        #elif defined( LILYGO_WATCH_2021 ) 
            switch( event ) {
                case POWERMGM_STANDBY:              log_d("button standby");
                                                    /*
                                                    * enable GPIO in lightsleep for wakeup
                                                    */
                                                    gpio_wakeup_enable( (gpio_num_t)BTN_1, GPIO_INTR_LOW_LEVEL );
                                                    gpio_wakeup_enable( (gpio_num_t)BTN_2, GPIO_INTR_LOW_LEVEL );
                                                    gpio_wakeup_enable( (gpio_num_t)BTN_3, GPIO_INTR_LOW_LEVEL );
                                                    esp_sleep_enable_gpio_wakeup ();
                                                    retval = true;
                                                    break;
                case POWERMGM_WAKEUP:               log_d("button wakeup");
                                                    retval = true;
                                                    break;
                case POWERMGM_SILENCE_WAKEUP:       log_d("button silence wakeup");
                                                    retval = true;
                                                    break;
                case POWERMGM_ENABLE_INTERRUPTS:    log_d("button enable interrupts");
                                                    retval = true;
                                                    break;
                case POWERMGM_DISABLE_INTERRUPTS:   log_d("button disable interrupts");
                                                    retval = true;
                                                    break;
            }
        #elif defined( WT32_SC01 )
            retval = true;
        #endif
    #endif

    return( retval );
}

bool button_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    /*
     * check if an callback table exist, if not allocate a callback table
     */
    if ( button_callback == NULL ) {
        button_callback = callback_init( "button" );
        if ( button_callback == NULL ) {
            log_e("button_callback alloc failed");
            while( true );
        }
    }
    /*
     * register an callback entry and return them
     */
    return( callback_register( button_callback, event, callback_func, id ) );
}

bool button_send_cb( EventBits_t event, void *arg ) {
    log_d("send button cb");
    /*
     * call all callbacks with her event mask
     */
    return( callback_send( button_callback, event, arg ) );
}