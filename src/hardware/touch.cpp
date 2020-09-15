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

lv_indev_t *touch_indev = NULL;

static bool touch_read(lv_indev_drv_t * drv, lv_indev_data_t*data);
static bool touch_getXY( int16_t &x, int16_t &y );
bool touch_powermgm_event_cb( EventBits_t event, void *arg );

void touch_setup( void ) {
    touch_indev = lv_indev_get_next( NULL );
    touch_indev->driver.read_cb = touch_read;

    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, touch_powermgm_event_cb, "touch" );
}

bool touch_powermgm_event_cb( EventBits_t event, void *arg ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    switch( event ) {
        case POWERMGM_STANDBY:          log_i("go standby");
                                        ttgo->touch->enterSleepMode();
                                        break;
        case POWERMGM_WAKEUP:           log_i("go wakeup");
                                        ttgo->touch->enterMonitorMode();
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   log_i("go silence wakeup");
                                        ttgo->touch->enterSleepMode();
                                        break;
    }
    return( true );
}

static bool touch_getXY( int16_t &x, int16_t &y ) {
    
    TTGOClass *ttgo = TTGOClass::getWatch();
    TP_Point p;
    static bool touch_press = false;

    // disable touch when we are in standby or silence wakeup
    if ( powermgm_get_event( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP ) ) {
        return( false );
    }

    if ( !ttgo->touch->touched() ) {
        touch_press = false;
        return( false );
    }

    if ( !touch_press ) {
        touch_press = true;
        motor_vibe( 1 );
    }

    p = ttgo->touch->getPoint();

    uint8_t rotation = ttgo->tft->getRotation();
    switch ( rotation ) {
    case 0:
        x = TFT_WIDTH - p.x;
        y = TFT_HEIGHT - p.y;
        break;
    case 1:
        x = TFT_WIDTH - p.y;
        y = p.x;
        break;
    case 3:
        x = p.y;
        y = TFT_HEIGHT - p.x;
        break;
    case 2:
    default:
        x = p.x;
        y = p.y;
    }

    // issue https://github.com/sharandac/My-TTGO-Watch/issues/18 fix
    float temp_x = ( x - ( lv_disp_get_hor_res( NULL ) / 2 ) ) * 1.15;
    float temp_y = ( y - ( lv_disp_get_ver_res( NULL ) / 2 ) ) * 1.0;
    x = temp_x + ( lv_disp_get_hor_res( NULL ) / 2 );
    y = temp_y + ( lv_disp_get_ver_res( NULL ) / 2 );

    return( true );
}

static bool touch_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
    data->state = touch_getXY(data->point.x, data->point.y) ?  LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    return( false );
}