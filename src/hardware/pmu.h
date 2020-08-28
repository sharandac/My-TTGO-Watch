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
#ifndef _PMU_H
    #define _PMU_H

    #include "TTGO.h"

    #define PMU_EVENT_AXP_INT       _BV(0)

    #define PMU_CONFIG_FILE         "/pmu.cfg"
    #define PMU_JSON_CONFIG_FILE    "/pmu.json"

    typedef struct {
        int32_t designed_battery_cap = 300;
        int32_t silence_wakeup_time = 60;
        int32_t silence_wakeup_time_vbplug = 3;
        int32_t normal_voltage = 3300;
        int32_t normal_power_save_voltage = 3000;
        int32_t experimental_normal_voltage = 3000;
        int32_t experimental_power_save_voltage = 2700;
        bool high_charging_target_voltage = true;
        bool compute_percent = false;
        bool experimental_power_save = false;
        bool silence_wakeup = true;
    } pmu_config_t;

    /*
     * @brief setup pmu: axp202
     */
    void pmu_setup( void );
    /*
     * @brief pmu loop routine, call from powermgm. not for user use
     */
    void pmu_loop( void );
    /*
     * @brief get the charge of battery in percent
     * 
     * @return  charge in percent or -1 if unknown
     */
    int32_t pmu_get_battery_percent( void );
    /*
     * @brief set the axp202 in standby
     */
    void pmu_standby( void );
    /*
     * @brief wakeup to axp202
     */
    void pmu_wakeup( void );
    /*
     * @brief save the config structure to SPIFFS
     */
    void pmu_save_config( void );
    /*
     * @brief read the config structure from SPIFFS
     */
    void pmu_read_config( void );
    /*
     * @brief read the config for calculated mAh based on the axp202 coloumb counter
     */
    bool pmu_get_calculated_percent( void );
    /*
     * @brief read the config for experimental power save
     * 
     * @return  true means enable, false means disable
     */
    bool pmu_get_experimental_power_save( void );
    /*
     * @brief set the config to use calculated mAh
     * 
     * @param   value   true enable calculated percent, false use AXP202 percent
     */
    void pmu_set_calculated_percent( bool value );
    /*
     * @brief set experimental power save
     * 
     * @param   value   true enable experiemental power save settings
     */
    void pmu_set_experimental_power_save( bool value );
    /*
     * @brief get the designed battery capacity in mAh
     * 
     * @return  capacity in mAh
     */
    int32_t pmu_get_designed_battery_cap( void );
    /*
     * @brief   get the current silence wakeup configuration,
     *          the time can configure in pmu.json
     * 
     * @return  true means enable, false means disable
     * 
     */
    bool pmu_get_silence_wakeup( void );
    /*
     * @brief set the current silence wakeup configureation
     * 
     * @param   value   true means enable, false means disable
     */
    void pmu_set_silence_wakeup( bool value );
    /*
     * @breif get the current battery voltage in mV
     * 
     * @return  voltage in mV
     */ 
    float pmu_get_battery_voltage( void );
    /*
     * @brief   get the batterty charging current in mA
     * 
     * @return  current in mA
     */
    float pmu_get_battery_charge_current( void );
    /*
     * @brief   get the battery discharging current in mA
     * 
     * @return  discharging current in mA
     */
    float pmu_get_battery_discharge_current( void );
    /*
     * @brief   get the VBUS voltage in mV
     * 
     * @return  VBUS voltage in mV
     */
    float pmu_get_vbus_voltage( void );
    /*
     * @brief   get battery capacity in mAh
     * 
     * @return  capacity in mAh
     */
    float pmu_get_coulumb_data( void );
    /*
     * @brief   get the charging state
     * 
     * @return  true means charging, false means no charging
     */
    bool pmu_is_charging( void );
    /*
     * @brief   get the vbus plug state
     * 
     * @return  true means plugged, false means not plugged
     */
    bool pmu_is_vbus_plug( void );

#endif // _PMU_H