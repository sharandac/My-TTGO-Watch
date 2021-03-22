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
#include "touch.h"
#include "powermgm.h"
#include "motor.h"
#include "display.h"

volatile bool DRAM_ATTR touch_irq_flag = false;
portMUX_TYPE DRAM_ATTR Touch_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR touch_irq( void );

bool touched = false;

lv_indev_t *touch_indev = NULL;

static bool touch_read(lv_indev_drv_t * drv, lv_indev_data_t*data);
static bool touch_getXY( int16_t &x, int16_t &y );
bool touch_powermgm_event_cb( EventBits_t event, void *arg );

static SemaphoreHandle_t xSemaphores = NULL;

void touch_setup( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    // This changes to polling mode.
    // The touch sensor holds the line low for the duration of the touch.
    // The level change can still trigger an interrupt, which we
    // use to start polling, and we don't stop polling till the level is high again.
    ttgo->touch->disableINT();
    ttgo->disableTouchIRQ();
    // This doesn't appear to change anything,
    // the sensor doesn't automatically switch to monitor mode till after 30 seconds
    ttgo->touch->setMonitorTime(0x01);
    // This is supposed to control how often the sensor checks for touch while in monitor mode
    // The units is ms between checks, so 250 checks only 4 times a second. 
    ttgo->touch->setMonitorPeriod(250);

    xSemaphores = xSemaphoreCreateMutex();
    touch_indev = lv_indev_get_next( NULL );
    touch_indev->driver.read_cb = touch_read;
    attachInterrupt( TOUCH_INT, &touch_irq, FALLING );

    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_ENABLE_INTERRUPTS | POWERMGM_DISABLE_INTERRUPTS , touch_powermgm_event_cb, "touch" );
}

bool touch_lock_take( void ) {
    return xSemaphoreTake( xSemaphores, portMAX_DELAY ) ==  pdTRUE;
}
void touch_lock_give( void ) {
    xSemaphoreGive( xSemaphores );
}

bool touch_powermgm_event_cb( EventBits_t event, void *arg ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    switch( event ) {
        case POWERMGM_STANDBY:          log_i("go standby");
                                        if ( touch_lock_take() ) {
                                            ttgo->touchToMonitor();
                                            touch_lock_give();
                                        }
                                        break;
        case POWERMGM_WAKEUP:           log_i("go wakeup");
                                        if ( touch_lock_take() ) {
                                            ttgo->touchToMonitor();
                                            touch_lock_give();
                                        }
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   log_i("go silence wakeup");
                                        break;
        case POWERMGM_ENABLE_INTERRUPTS:
                                        attachInterrupt( TOUCH_INT, &touch_irq, FALLING );
                                        break;
        case POWERMGM_DISABLE_INTERRUPTS:
                                        detachInterrupt( TOUCH_INT );
                                        break;
    }
    return( true );
}

static bool touch_getXY( int16_t &x, int16_t &y ) {
    
    TTGOClass *ttgo = TTGOClass::getWatch();
    static bool touch_press = false;

    // disable touch when we are in standby or silence wakeup
    if ( powermgm_get_event( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP ) ) {
        return( false );
    }

    bool getTouchResult = false;
    if ( touch_lock_take() ) {
        getTouchResult = ttgo->getTouch( x, y );
        touch_lock_give();
    }
    if ( !getTouchResult ) {
        touch_press = false;
        return( false );
    }

    if ( !touch_press ) {
        touch_press = true;
        if ( display_get_vibe() )
            motor_vibe( 3 );
    }

    // issue https://github.com/sharandac/My-TTGO-Watch/issues/18 fix
    float temp_x = ( x - ( lv_disp_get_hor_res( NULL ) / 2 ) ) * 1.15;
    float temp_y = ( y - ( lv_disp_get_ver_res( NULL ) / 2 ) ) * 1.0;
    x = temp_x + ( lv_disp_get_hor_res( NULL ) / 2 );
    y = temp_y + ( lv_disp_get_ver_res( NULL ) / 2 );

    return( true );
}

void IRAM_ATTR touch_irq( void ) {
    portENTER_CRITICAL_ISR(&Touch_IRQ_Mux);
    touch_irq_flag = true;
    portEXIT_CRITICAL_ISR(&Touch_IRQ_Mux);
}

static bool touch_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
    // We use two flags, one changes in the interrupt handler
    // the other controls whether we poll the sensor,
    // and gets cleared when the level is no longer low,
    // meaning the touch has finished
    portENTER_CRITICAL( &Touch_IRQ_Mux );
    bool temp_touch_irq_flag = touch_irq_flag;
    touch_irq_flag = false;
    portEXIT_CRITICAL( &Touch_IRQ_Mux );
    touched |= temp_touch_irq_flag;
    if (touched) {
        data->state = touch_getXY(data->point.x, data->point.y) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
        touched=digitalRead(TOUCH_INT) == LOW;
        if (!touched) {
            // Save power by switching to monitor mode now instead of waiting for 30 seconds.
            if ( touch_lock_take() ) {
                TTGOClass::getWatch()->touchToMonitor();
                touch_lock_give();
            }
        } else {
            log_i("touched @ %d,%d %d", data->point.x, data->point.y, data->state );
        }
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
    return( false );
}