
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

#include "watchface_expr.h"
#include "hardware/pmu.h"
#include "hardware/bma.h"
#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "time.h"

static double time_hour;
static double time_min;
static double time_sec;

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