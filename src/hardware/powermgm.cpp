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
#include <TTGO.h>
#include <soc/rtc.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <time.h>
#include "driver/adc.h"
#include "esp_pm.h"


#include "pmu.h"
#include "bma.h"
#include "powermgm.h"
#include "wifictl.h"
#include "blectl.h"
#include "timesync.h"
#include "motor.h"
#include "touch.h"
#include "display.h"
#include "sound.h"

#include "gui/mainbar/mainbar.h"

EventGroupHandle_t powermgm_status = NULL;
portMUX_TYPE powermgmMux = portMUX_INITIALIZER_UNLOCKED;

/*
 *
 */
void powermgm_setup( TTGOClass *ttgo ) {

    powermgm_status = xEventGroupCreate();

    pmu_setup( ttgo );
    bma_setup( ttgo );
    wifictl_setup();
    blectl_read_config();
    timesync_setup( ttgo );
    touch_setup( ttgo );
    sound_setup();
}

/*
 *
 */
void powermgm_loop( TTGOClass *ttgo ) {

    // check if a button or doubleclick was release
    if( powermgm_get_event( POWERMGM_PMU_BUTTON | POWERMGM_BMA_DOUBLECLICK ) ) {
        if ( powermgm_get_event( POWERMGM_STANDBY ) || powermgm_get_event( POWERMGM_SILENCE_WAKEUP ) ) {
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
        }
        else {
            powermgm_set_event( POWERMGM_STANDBY_REQUEST );
        }
        powermgm_clear_event( POWERMGM_PMU_BUTTON | POWERMGM_BMA_DOUBLECLICK );
    }

    if ( powermgm_get_event( POWERMGM_WAKEUP_REQUEST ) && powermgm_get_event( POWERMGM_WAKEUP ) ) {
        lv_disp_trig_activity( NULL );
        powermgm_clear_event( POWERMGM_WAKEUP_REQUEST );
    }
  
    // drive into
    if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP_REQUEST | POWERMGM_WAKEUP_REQUEST ) ) {
        powermgm_clear_event( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP );

        log_i("go wakeup");

        setCpuFrequencyMhz(240);

        pmu_wakeup();
        bma_wakeup();
        display_wakeup( powermgm_get_event( POWERMGM_SILENCE_WAKEUP_REQUEST )?true:false );

        timesyncToSystem();

        wifictl_wakeup();
        blectl_wakeup();

        log_i("Free heap: %d", ESP.getFreeHeap());
        log_i("Free PSRAM heap: %d", ESP.getFreePsram());
        log_i("uptime: %d", millis() / 1000 );

        ttgo->startLvglTick();
        lv_disp_trig_activity(NULL);

        if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP_REQUEST ) ) {
            powermgm_set_event( POWERMGM_SILENCE_WAKEUP );
        }
        else {
            powermgm_set_event( POWERMGM_WAKEUP );
        }
    }        
    else if( powermgm_get_event( POWERMGM_STANDBY_REQUEST ) ) {
        powermgm_clear_event( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP );

        if ( !display_get_block_return_maintile() ) {
            mainbar_jump_to_maintile( LV_ANIM_OFF );
        }

        ttgo->stopLvglTick();

        log_i("Free heap: %d", ESP.getFreeHeap());
        log_i("Free PSRAM heap: %d", ESP.getFreePsram());
        log_i("uptime: %d", millis() / 1000 );

        display_standby();

        timesyncToRTC();

        bma_standby();
        pmu_standby();
        wifictl_standby();
        blectl_standby();

        adc_power_off();

        powermgm_set_event( POWERMGM_STANDBY );

        if ( !blectl_get_enable_on_standby() ) {
            motor_vibe(3);
            delay(50);
            log_i("go standby");
            setCpuFrequencyMhz( 10 );
            gpio_wakeup_enable ( (gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL );
            gpio_wakeup_enable ( (gpio_num_t)BMA423_INT1, GPIO_INTR_HIGH_LEVEL );
            esp_sleep_enable_gpio_wakeup ();
            esp_light_sleep_start();
            // from here, the consumption is round about 2.5mA
            // total standby time is 152h (6days) without use?
        }
        else {
            log_i("standby block by bluetooth");
            setCpuFrequencyMhz( 80 );
            // from here, the consumption is round about 23mA
            // total standby time is 19h without use?
        }
    }
    powermgm_clear_event( POWERMGM_SILENCE_WAKEUP_REQUEST | POWERMGM_WAKEUP_REQUEST | POWERMGM_STANDBY_REQUEST );

    pmu_loop( ttgo );
    bma_loop( ttgo );
    display_loop( ttgo );
}

/*
 *
 */
void powermgm_set_event( EventBits_t bits ) {
    portENTER_CRITICAL(&powermgmMux);
    xEventGroupSetBits( powermgm_status, bits );
    portEXIT_CRITICAL(&powermgmMux);
}

/*
 *
 */
void powermgm_clear_event( EventBits_t bits ) {
    portENTER_CRITICAL(&powermgmMux);
    xEventGroupClearBits( powermgm_status, bits );
    portEXIT_CRITICAL(&powermgmMux);
}

/*
 *
 */
EventBits_t powermgm_get_event( EventBits_t bits ) {
    portENTER_CRITICAL(&powermgmMux);
    EventBits_t temp = xEventGroupGetBits( powermgm_status ) & bits;
    portEXIT_CRITICAL(&powermgmMux);
    return( temp );
}
