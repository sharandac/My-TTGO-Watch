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
#include <stdio.h>
#include <time.h>

#include "motion.h"
#include "powermgm.h"
#include "callback.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"

    static uint32_t stepcounter_valid;                      /** @brief stepcount valid mask, if 0xa5a5a5a5 when stepcounter is valid after reset */
    static uint32_t stepcounter_before_reset;               /** @brief stepcounter before reset */
    static uint32_t stepcounter;                            /** @brief stepcounter */
#else
    #if defined( M5PAPER )

    #elif defined( M5CORE2 )

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <TTGO.h>
    #elif defined( LILYGO_WATCH_2021 )
        #include <twatch2021_config.h>
/*
        #include <bma423/bma.h>
        #include <i2c/i2c_bus.h>

        I2CBus *i2c = NULL;
        BMA *bma = NULL;
*/
    #else
        #warning "not hardware driver for bma/axis sensor"
    #endif
    #include <soc/rtc.h>

    volatile bool DRAM_ATTR bma_irq_flag = false;
    portMUX_TYPE DRAM_ATTR BMA_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;
    /**
     * move internal stepcounter into noninit ram section
     */
    __NOINIT_ATTR uint32_t stepcounter_valid;               /** @brief stepcount valid mask, if 0xa5a5a5a5 when stepcounter is valid after reset */
    __NOINIT_ATTR uint32_t stepcounter_before_reset;        /** @brief stepcounter before reset */
    __NOINIT_ATTR uint32_t stepcounter;                     /** @brief stepcounter */

    void IRAM_ATTR bma_irq( void );
    void IRAM_ATTR bma_irq( void ) {
        portENTER_CRITICAL_ISR(&BMA_IRQ_Mux);
        bma_irq_flag = true;
        portEXIT_CRITICAL_ISR(&BMA_IRQ_Mux);
    }
#endif

bma_config_t bma_config;
callback_t *bma_callback = NULL;

bool first_loop_run = true;

bool bma_send_event_cb( EventBits_t event, void *arg );
bool bma_powermgm_event_cb( EventBits_t event, void *arg );
bool bma_powermgm_loop_cb( EventBits_t event, void *arg );
void bma_notify_stepcounter( void );

void bma_setup( void ) {
    /*
     * check if stepcounter valid and reset if not valid
     */
    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
            /**
             * forec reset stepcounter on M5PAPER
             */
            stepcounter_valid = 0;
            stepcounter_before_reset = 0;
            stepcounter = 0;
        #elif defined( M5CORE2 )
            /**
             * forec reset stepcounter on M5PAPER
             */
            stepcounter_valid = 0;
            stepcounter_before_reset = 0;
            stepcounter = 0;        
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            if ( stepcounter_valid != 0xa5a5a5a5 ) {
                stepcounter = 0;
                stepcounter_before_reset = 0;
                stepcounter_valid = 0xa5a5a5a5;
                bma_send_event_cb( BMACTL_STEPCOUNTER_RESET, NULL );
                log_i("stepcounter not valid. reset");
            }
            stepcounter = stepcounter + stepcounter_before_reset;
        #elif defined( LILYGO_WATCH_2021 )
/*
            if ( stepcounter_valid != 0xa5a5a5a5 ) {
                stepcounter = 0;
                stepcounter_before_reset = 0;
                stepcounter_valid = 0xa5a5a5a5;
                bma_send_event_cb( BMACTL_STEPCOUNTER_RESET, NULL );
                log_i("stepcounter not valid. reset");
            }
            stepcounter = stepcounter + stepcounter_before_reset;
*/
        #endif
    #endif
    /*
     * load config from json
     */
    bma_config.load();
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            /*
             * init stepcounter
             */
            ttgo->bma->begin();
            ttgo->bma->attachInterrupt();
            ttgo->bma->direction();
            /*
             * init stepcounter interrupt function
             */
            pinMode( BMA423_INT1, INPUT );
            attachInterrupt( BMA423_INT1, bma_irq, RISING );
        #elif defined( LILYGO_WATCH_2021 )
/*
            i2c = new I2CBus( Wire, 26, 25 );
            bma = new BMA( *i2c );
            
            bma->begin();
            bma->attachInterrupt();
            bma->direction();
*/
            /*
             * init stepcounter interrupt function
             */
//            pinMode( BMA_INT_1, INPUT );
//            attachInterrupt( BMA_INT_1, bma_irq, RISING );
        #endif
    #endif
    /*
     * load config setting for tilt, stepcounter and wakeup to enabled interrupts
     */
    bma_reload_settings();
    /*
     * register powermgm callback funtions
     */
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_ENABLE_INTERRUPTS | POWERMGM_DISABLE_INTERRUPTS , bma_powermgm_event_cb, "powermgm bma" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, bma_powermgm_loop_cb, "powermgm bma loop" );
}

bool bma_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:          bma_standby();
                                        break;
        case POWERMGM_WAKEUP:           bma_wakeup();
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   bma_wakeup();
                                        break;
        #ifdef NATIVE_64BIT
        #else
            #ifdef M5PAPER
            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                case POWERMGM_ENABLE_INTERRUPTS:
                                                attachInterrupt( BMA423_INT1, bma_irq, RISING );
                                                break;
                case POWERMGM_DISABLE_INTERRUPTS:
                                                detachInterrupt( BMA423_INT1 );
                                                break;
            #elif defined( LILYGO_WATCH_2021 )
/*
                case POWERMGM_ENABLE_INTERRUPTS:
                                                attachInterrupt( BMA_INT_1, bma_irq, RISING );
                                                break;
                case POWERMGM_DISABLE_INTERRUPTS:
                                                detachInterrupt( BMA_INT_1 );
                                                break;
*/
            #endif
        #endif
    }
    return( true );
}

bool bma_powermgm_loop_cb( EventBits_t event , void *arg ) {
    static bool first_powermgm_loop_cb = true;
    static bool BMA_tilt = false;
    static bool BMA_doubleclick = false;
    static bool BMA_stepcounter = false;
    bool temp_bma_irq_flag = false;
    /*
     * handle IRQ event
     */
    #ifdef NATIVE_64BIT
    #else
        portENTER_CRITICAL(&BMA_IRQ_Mux);
        temp_bma_irq_flag = bma_irq_flag;
        bma_irq_flag = false;
        portEXIT_CRITICAL(&BMA_IRQ_Mux);
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
    #endif
    /*
     * check interrupts event source
     */
    if ( temp_bma_irq_flag ) {
        #ifdef NATIVE_64BIT
        #else
            #ifdef M5PAPER
            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                TTGOClass *ttgo = TTGOClass::getWatch();
                while( !ttgo->bma->readInterrupt() );
                /*
                * set powermgm wakeup event and save BMA_* event
                */
                if ( ttgo->bma->isDoubleClick() ) {
                    if ( !powermgm_get_event( POWERMGM_WAKEUP ) )
                        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                    BMA_doubleclick = true;
                }
                if ( ttgo->bma->isTilt() ) {
                    if ( !powermgm_get_event( POWERMGM_WAKEUP ) )
                        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                    BMA_tilt = true;
                }
                if ( ttgo->bma->isStepCounter() ) {
                    BMA_stepcounter = true;
                }
            #elif defined( LILYGO_WATCH_2021 )
                /*
                * set powermgm wakeup event and save BMA_* event
                */
/*
                while( !bma->readInterrupt() );
                if ( bma->isDoubleClick() ) {
                    if ( !powermgm_get_event( POWERMGM_WAKEUP ) )
                        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                    BMA_doubleclick = true;
                }
                if ( bma->isTilt() ) {
                    if ( !powermgm_get_event( POWERMGM_WAKEUP ) )
                        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                    BMA_tilt = true;
                }
                if ( bma->isStepCounter() ) {
                    BMA_stepcounter = true;
                }
*/
            #endif
        #endif         
    }
    /*
     * check pmu event
     */
    switch( event ) {
        case POWERMGM_WAKEUP:   {
            /*
             * check if an BMA_* event triggered
             * one event per loop
             */
            if ( BMA_doubleclick ) {
                BMA_doubleclick = false;
                bma_send_event_cb( BMACTL_DOUBLECLICK, NULL );
            }
            else if ( BMA_tilt ) {
                BMA_tilt = false;
                bma_send_event_cb( BMACTL_TILT, NULL );
            }
            else if ( BMA_stepcounter ) {
                BMA_stepcounter = false;
                bma_notify_stepcounter();
            }
            break;
        }
    }
    /*
     * if it the first powermgm loop cb call
     * update stepcounter
     */
    if ( first_powermgm_loop_cb ) {
        first_powermgm_loop_cb = false;
        bma_notify_stepcounter();
    }
    return( true );
}

void bma_notify_stepcounter( void ) {
    uint32_t val = 0;
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            stepcounter_before_reset = ttgo->bma->getCounter();
        #elif defined( LILYGO_WATCH_2021 )
//            stepcounter_before_reset = bma->getCounter();
        #endif
    #endif
    val = stepcounter + stepcounter_before_reset;
    bma_send_event_cb( BMACTL_STEPCOUNTER, &val );
}

void bma_standby( void ) {
    log_i("go standby");
    /*
     * disable stepcounter interrupt to avoid
     * wakeup in standby mode
     */
    if ( bma_get_config( BMA_STEPCOUNTER ) ) {
        #ifdef NATIVE_64BIT
        #else
            #ifdef M5PAPER
            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->bma->enableStepCountInterrupt( false );
            #elif defined( LILYGO_WATCH_2021 )
//                bma->enableStepCountInterrupt( false );
            #endif
        #endif
    }
    /*
     * enable interrupt in ESP32 sleepmode
     */
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            gpio_wakeup_enable ( (gpio_num_t)BMA423_INT1, GPIO_INTR_HIGH_LEVEL );
            esp_sleep_enable_gpio_wakeup ();
        #elif defined( LILYGO_WATCH_2021 )
//            gpio_wakeup_enable ( (gpio_num_t)BMA_INT_1, GPIO_INTR_ANYEDGE );
//            esp_sleep_enable_gpio_wakeup ();
        #endif
    #endif
}

void bma_wakeup( void ) {
    log_i("go wakeup");
    /*
     * enable stepcounter interrupt for updates
     * when the user interacts with the watch
     */
    if ( bma_get_config( BMA_STEPCOUNTER ) ) {
        #ifdef NATIVE_64BIT
        #else
            #ifdef M5PAPER
            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->bma->enableStepCountInterrupt( true );
            #elif defined( LILYGO_WATCH_2021 )
//                bma->enableStepCountInterrupt( true );
            #endif
        #endif
    }
    /*
     * check for a new day and reset stepcounter if configure
     */
    if ( bma_get_config( BMA_DAILY_STEPCOUNTER ) ) {
        static bool first_wakeup = true;
        static struct tm old_info;
        /**
         * get local time
         */
        time_t now;
        tm info;
        time( &now );
        localtime_r( &now, &info );
        /**
         * on first wakeup, init old_info
         */
        if ( first_wakeup ) {
            first_wakeup = false;
            localtime_r( &now, &old_info );
        }
        /**
         * check if day has change
         */
        if ( info.tm_yday != old_info.tm_yday ) {
            log_i("reset setcounter: %d != %d", info.tm_yday, old_info.tm_yday );
            #ifdef NATIVE_64BIT
            #else
                #ifdef M5PAPER
                #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                    TTGOClass *ttgo = TTGOClass::getWatch();
                    ttgo->bma->resetStepCounter();
                #elif defined( LILYGO_WATCH_2021 )
//                    bma->resetStepCounter();
                #endif
            #endif
            stepcounter_before_reset = 0;
            stepcounter = 0;
            bma_send_event_cb( BMACTL_STEPCOUNTER_RESET, NULL );
        }
        /*
         * store current time 
         */
        localtime_r( &now, &old_info );
    }
    /*
     * send bma stepcounter updates
     */
    bma_notify_stepcounter();
}

void bma_reload_settings( void ) {
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            ttgo->bma->enableStepCountInterrupt( bma_config.enable[ BMA_STEPCOUNTER ] );
            ttgo->bma->enableWakeupInterrupt( bma_config.enable[ BMA_DOUBLECLICK ] );
            ttgo->bma->enableTiltInterrupt( bma_config.enable[ BMA_TILT ] );
        #elif defined( LILYGO_WATCH_2021 )
//            bma->enableStepCountInterrupt( bma_config.enable[ BMA_STEPCOUNTER ] );
//            bma->enableWakeupInterrupt( bma_config.enable[ BMA_DOUBLECLICK ] );
//            bma->enableTiltInterrupt( bma_config.enable[ BMA_TILT ] );
        #endif
    #endif
}

bool bma_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( bma_callback == NULL ) {
        bma_callback = callback_init( "bma" );
        if ( bma_callback == NULL ) {
            log_e("bma_callback alloc failed");
            while(true);
        }
    }
    return( callback_register( bma_callback, event, callback_func, id ) );
}

bool bma_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send( bma_callback, event, arg ) );
}

void bma_save_config( void ) {
    bma_config.save();
}

void bma_read_config( void ) {
    bma_config.load();
}

bool bma_get_config( int config ) {
    return bma_config.get_config(config);
}

void bma_set_config( int config, bool enable ) {
    bma_config.bma_set_config( config, enable);
    bma_config.save();
    bma_reload_settings();
}

void bma_set_rotate_tilt( uint32_t rotation ) {
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            struct bma423_axes_remap remap_data;
            TTGOClass *ttgo = TTGOClass::getWatch();
            #if defined( LILYGO_WATCH_2020_V2 )
                /**
                 * fix bma orientation on V2
                 */
                rotation = rotation + 270;
            #endif
            rotation = rotation % 360;
            switch( rotation / 90 ) {
                case 0:     remap_data.x_axis = 0;
                            remap_data.x_axis_sign = 1;
                            remap_data.y_axis = 1;
                            remap_data.y_axis_sign = 1;
                            remap_data.z_axis  = 2;
                            remap_data.z_axis_sign  = 1;
                            ttgo->bma->set_remap_axes(&remap_data);
                            break;
                case 1:     remap_data.x_axis = 1;
                            remap_data.x_axis_sign = 1;
                            remap_data.y_axis = 0;
                            remap_data.y_axis_sign = 0;
                            remap_data.z_axis  = 2;
                            remap_data.z_axis_sign  = 1;
                            ttgo->bma->set_remap_axes(&remap_data);
                            break;
                case 2:     remap_data.x_axis = 0;
                            remap_data.x_axis_sign = 1;
                            remap_data.y_axis = 1;
                            remap_data.y_axis_sign = 0;
                            remap_data.z_axis  = 2;
                            remap_data.z_axis_sign  = 1;
                            ttgo->bma->set_remap_axes(&remap_data);
                            break;
                case 3:     remap_data.x_axis = 1;
                            remap_data.x_axis_sign = 1;
                            remap_data.y_axis = 0;
                            remap_data.y_axis_sign = 1;
                            remap_data.z_axis  = 2;
                            remap_data.z_axis_sign  = 1;
                            ttgo->bma->set_remap_axes(&remap_data);
                            break;
            }
        #elif defined( LILYGO_WATCH_2021 )
/*
            struct bma423_axes_remap remap_data;
            rotation = rotation % 360;
            switch( rotation / 90 ) {
                case 0:     remap_data.x_axis = 0;
                            remap_data.x_axis_sign = 1;
                            remap_data.y_axis = 1;
                            remap_data.y_axis_sign = 1;
                            remap_data.z_axis  = 2;
                            remap_data.z_axis_sign  = 1;
                            bma->set_remap_axes(&remap_data);
                            break;
                case 1:     remap_data.x_axis = 1;
                            remap_data.x_axis_sign = 1;
                            remap_data.y_axis = 0;
                            remap_data.y_axis_sign = 0;
                            remap_data.z_axis  = 2;
                            remap_data.z_axis_sign  = 1;
                            bma->set_remap_axes(&remap_data);
                            break;
                case 2:     remap_data.x_axis = 0;
                            remap_data.x_axis_sign = 1;
                            remap_data.y_axis = 1;
                            remap_data.y_axis_sign = 0;
                            remap_data.z_axis  = 2;
                            remap_data.z_axis_sign  = 1;
                            bma->set_remap_axes(&remap_data);
                            break;
                case 3:     remap_data.x_axis = 1;
                            remap_data.x_axis_sign = 1;
                            remap_data.y_axis = 0;
                            remap_data.y_axis_sign = 1;
                            remap_data.z_axis  = 2;
                            remap_data.z_axis_sign  = 1;
                            bma->set_remap_axes(&remap_data);
                            break;
            }
*/
        #endif
    #endif
}

uint32_t bma_get_stepcounter( void ) {
    return stepcounter + stepcounter_before_reset;
}

void bma_reset_stepcounter( void ) {
    log_i("reset step counter");
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            ttgo->bma->resetStepCounter();
        #elif defined( LILYGO_WATCH_2021 )
//            bma->resetStepCounter();
        #endif
    #endif
    /**
     * FIXME: why not required during daily reset?
     */
    stepcounter = 0;
    stepcounter_before_reset = 0;
    /*
     * Announce forced change
     */
    bma_notify_stepcounter();
}
