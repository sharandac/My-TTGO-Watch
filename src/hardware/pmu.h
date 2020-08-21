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
        bool high_charging_target_voltage = true;
        bool compute_percent = false;
        bool experimental_power_save = false;
        bool silence_wakeup = true;
    } pmu_config_t;

    /*
     * @brief setup pmu: axp202
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void pmu_setup( TTGOClass *ttgo );
    /*
     * @brief pmu loop routine, call from powermgm. not for user use
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void pmu_loop( TTGOClass *ttgo );
    /*
     *
     * @brief get the charge of battery in percent
     * 
     * @param   ttgo    pointer to an TTGOClass
     * 
     * @return  charge in percent or -1 if unknown
     */
    int32_t pmu_get_battery_percent( TTGOClass *ttgo );
    /*
     * @brief set the axp202 in standby
     */
    void pmu_standby( void );
    /*
     * @brief wakeup to axp202
     */
    void pmu_wakeup( void );
    /*
     * @ brief save the config structure to SPIFF
     */
    void pmu_save_config( void );
    /*
     * @ brief read the config structure from SPIFF
     */
    void pmu_read_config( void );
    /*
     * @brief read the config for calculated mAh based on the axp202 coloumb counter
     */
    bool pmu_get_calculated_percent( void );
    /*
     * @brief read the config for experimental power save
     */
    bool pmu_get_experimental_power_save( void );
    /*
     * @brief set the config to use calculated mAh
     */
    void pmu_set_calculated_percent( bool value );
    /*
     * @brief set experimental power save
     */
    void pmu_set_experimental_power_save( bool value );
    int32_t pmu_get_designed_battery_cap( void );
    bool pmu_get_silence_wakeup( void );
    void pmu_set_silence_wakeup( bool value );

#endif // _PMU_H