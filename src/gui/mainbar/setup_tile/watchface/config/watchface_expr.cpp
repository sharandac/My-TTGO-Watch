
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

/*
 * Some design elements.
 * The module offers acces to all the parts of the state of the watch (battery level, steps, wifi...).
 * The communication with watchface is in a pull driven model: the watchface regularly refresh its state
 * requesting current values from the watch state model.
 * Thus, for some part of the watch state model, data are immediately requested with a direct function call (steps, batt level),
 * while for other part, with a lower refresh rate, the state is cached (wifi status...).
 */

#include "watchface_expr.h"
#include "hardware/pmu.h"
#include "hardware/motion.h"
#include "hardware/gpsctl.h"
#include "hardware/wifictl.h"
#include "hardware/blectl.h"
#include "hardware/rtcctl.h"
#include "hardware/sound.h"
#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "time.h"

static double time_hour;
static double time_min;
static double time_sec;

// Sound
static double sound_volume = 0.;
static double sound_enabled = 0.;

// Wifi
static double wifi = 0; // disabled=0, enabled+disconnected=1, enabled+connected=2

// Bluetooth
static double ble = 0.; // disabled=0, enabled+disconnected=1, enabled+connected=2

// GPS
static double gps = 0.; // disabled=0, enabled(no fix)=1, enabled+fix=2

// Alarm
static double alarm_state = 0.;

extern "C" {
    /**
     * Adapter functions.
     * 
     * TinyExpr expect function returning double.
     */

    double get_battery_percent(void) {
        return pmu_get_battery_percent();
    }
    double get_battery_voltage(void) {
        return pmu_get_battery_voltage() / 1000;
    }
    double get_bluetooth_nb_msg(void) {
        return bluetooth_get_number_of_msg();
    }
    double get_stepcounter(void) {
        return bma_get_stepcounter();
    }
}

te_variable watchface_expr_vars[] = {
    {"gps", &gps},
    {"ble", &ble},
    {"sound_volume", &sound_volume},
    {"sound_enabled", &sound_enabled},
    {"alarm", &alarm_state},
    {"wifi", &wifi},
    {"battery_percent", (const void*)get_battery_percent, TE_FUNCTION0},
    {"battery_voltage", (const void*)get_battery_voltage, TE_FUNCTION0},
    {"bluetooth_messages", (const void*)get_bluetooth_nb_msg, TE_FUNCTION0},
    {"steps", (const void*)get_stepcounter, TE_FUNCTION0},
    {"time_hour", &time_hour},
    {"time_min", &time_min},
    {"time_sec", &time_sec}
};

void watchface_expr_update( tm &new_info ) {
    time_hour = new_info.tm_hour;
    time_min = new_info.tm_min;
    time_sec = new_info.tm_sec;
}

te_expr * watchface_expr_compile(const char* str, int *error) {
    return te_compile(str, watchface_expr_vars, sizeof(watchface_expr_vars), error);
}

double watchface_expr_eval( te_expr *expr) {
    return te_eval( expr );
}

bool watchface_expr_gpsctl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case GPSCTL_DISABLE:
            gps = 0.;
            break;
        case GPSCTL_ENABLE:
            gps = 1.;
            break;
        case GPSCTL_FIX:
            gps = 2.;
            break;
        case GPSCTL_NOFIX:
            gps = 1.;
            break;
    }
    return( true );
}

bool watchface_expr_soundctl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case SOUNDCTL_ENABLED:
            sound_enabled = *(bool*)arg;
            break;
        case SOUNDCTL_VOLUME:
            sound_volume = *(uint8_t*)arg;
            break;
    }
    return( true );
}

bool watchface_expr_rtcctl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case RTCCTL_ALARM_ENABLED:
            alarm_state = 1.;
            break;
        case RTCCTL_ALARM_DISABLED:
            alarm_state = 0.;
            break;
    }
    return( true );
}

bool watchface_expr_blectl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_ON:
            ble = 1.;
            break;
        case BLECTL_OFF:
            ble = 0.;
            break;
        case BLECTL_CONNECT:
            ble = 2.;
            break;
        case BLECTL_DISCONNECT:
            ble = 1.;
            break;
    }
    return( true );
}

bool watchface_expr_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT:
            wifi = 2.;
            break;
        case WIFICTL_DISCONNECT:
            wifi = 1.;
            break;
        case WIFICTL_OFF:
            wifi = 0.;
            break;
        case WIFICTL_ON:
            wifi = 1.;
            break;
    }
    return( true );
}

void watchface_expr_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    blectl_register_cb( BLECTL_CONNECT | BLECTL_DISCONNECT | BLECTL_ON | BLECTL_OFF, watchface_expr_blectl_event_cb, "bluetooth state" );
    wifictl_register_cb( WIFICTL_CONNECT | WIFICTL_DISCONNECT | WIFICTL_OFF | WIFICTL_ON, watchface_expr_wifictl_event_cb, "wifi state" );
    rtcctl_register_cb( RTCCTL_ALARM_ENABLED | RTCCTL_ALARM_DISABLED, watchface_expr_rtcctl_event_cb, "rtc state" );
    sound_register_cb( SOUNDCTL_ENABLED | SOUNDCTL_VOLUME, watchface_expr_soundctl_event_cb, "sound state");
    gpsctl_register_cb( GPSCTL_DISABLE | GPSCTL_ENABLE | GPSCTL_FIX | GPSCTL_NOFIX, watchface_expr_gpsctl_event_cb, "gps state" );
}
