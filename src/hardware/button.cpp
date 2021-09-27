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
    #ifdef M5PAPER
        #include <M5EPD.h>
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
                                            log_i("send BUTTON_PWR event");
                                            break;
                case PMUCTL_LONG_PRESS:     button_send_cb( BUTTON_QUICKBAR, (void *)NULL );
                                            log_i("send BUTTON_QUICKBAR event");
                                            break;
            }
            return( true );
        }
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
    }
#endif

callback_t *button_callback = NULL;
bool button_powermgm_loop_cb( EventBits_t event, void *arg );
bool button_powermgm_event_cb( EventBits_t event, void *arg );

void button_setup( void ) {
    #ifdef NATIVE_64BIT

    #else
        #ifdef M5PAPER
            /**
             * set push button IO
             */
            attachInterrupt( M5EPD_KEY_PUSH_PIN, button_irq, FALLING );
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            /**
             * special case: on ttgo watch 2020 the button is connected 
             * to the pmu
             */
            pmu_register_cb( PMUCTL_SHORT_PRESS | PMUCTL_LONG_PRESS, button_pmu_event_cb, "button pmu event" );
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
            log_i("button was pressed");
            push_presstime = millis();
        }
        else if( M5.BtnP.wasReleased() ) {
            log_i("button was release");
            if( !temp_button_irq_flag ) {
                push_presstime = millis() - push_presstime;
                log_i("presstime = %dms", push_presstime );
                if ( push_presstime < 2000 ) {
                    log_i("short press");
                    button_send_cb( BUTTON_PWR, (void *)NULL );
                }
                else if  ( push_presstime < 4500 ) {
                    log_i("long press");
                    button_send_cb( BUTTON_QUICKBAR, (void *)NULL );           
                }
                else {
                    log_i("press state ignore");
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
                log_i("left press");
                button_send_cb( BUTTON_LEFT, (void *)NULL );
            }
            /**
             * handle right button event
             */
            if ( M5.BtnR.wasPressed() ) {
                log_i("right press");
                button_send_cb( BUTTON_RIGHT, (void *)NULL );
            }
        }
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif
#endif
    return( true );
}

bool button_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = false;

    #ifdef NATIVE_64BIT
        retval = true;
    #else
        #ifdef M5PAPER
            switch( event ) {
                case POWERMGM_STANDBY:              log_i("button standby");
                                                    /*
                                                    * enable GPIO in lightsleep for wakeup
                                                    */
                                                    gpio_wakeup_enable( (gpio_num_t)M5EPD_KEY_PUSH_PIN, GPIO_INTR_LOW_LEVEL );
                                                    esp_sleep_enable_gpio_wakeup ();
                                                    retval = true;
                                                    break;
                case POWERMGM_WAKEUP:               log_i("button wakeup");
                                                    retval = true;
                                                    break;
                case POWERMGM_SILENCE_WAKEUP:       log_i("button silence wakeup");
                                                    retval = true;
                                                    break;
                case POWERMGM_ENABLE_INTERRUPTS:    log_i("button enable interrupts");
                                                    attachInterrupt( M5EPD_KEY_PUSH_PIN, &button_irq, FALLING );
                                                    retval = true;
                                                    break;
                case POWERMGM_DISABLE_INTERRUPTS:   log_i("button disable interrupts");
                                                    detachInterrupt( M5EPD_KEY_PUSH_PIN );
                                                    retval = true;
                                                    break;
            }
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
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
    /*
     * call all callbacks with her event mask
     */
    return( callback_send( button_callback, event, arg ) );
}