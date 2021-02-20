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
    #include "callback.h"

	/**
     * PMU events mask
     */
    #define PMUCTL_STATUS                   _BV(0)              /** @brief event mask for pmuctl battery status update, callback arg is (int32_t*) */
    #define PMUCTL_SHORT_PRESS              _BV(1)              /** @brief event mask for pmuctl short press, no callback arg */
    #define PMUCTL_LONG_PRESS               _BV(2)              /** @brief event mask for pmuctl long press, nocallback arg */
    #define PMUCTL_TIMER_TIMEOUT            _BV(3)              /** @brief event mask for pmuctl timer timeout, no callback arg */
    /**
     * PMU status mask
     */
    #define PMUCTL_STATUS_PERCENT  0xFF
    #define PMUCTL_STATUS_PLUG     0x100
    #define PMUCTL_STATUS_CHARGING 0x200
	/**
     * Some default values, used below as well as in pmu.cpp during json reads
     */
    #define SILENCEWAKEINTERVAL             45                  /** @brief defines the silence wakeup interval in minutes */
	#define SILENCEWAKEINTERVAL_PLUG		3                   /** @brief defines the silence wakeup interval in minutes when plugged*/
    #define NORMALVOLTAGE                   3300                /** @brief defines the norminal voltages while working */
    #define NORMALPOWERSAVEVOLTAGE          3000                /** @brief defines the norminal voltages while in powersave */
    #define EXPERIMENTALNORMALVOLTAGE       3000                /** @brief defines the norminal voltages while working with exprimental powersave enabled */
    #define EXPERIMENTALPOWERSAVEVOLTAGE    2800                /** @brief defines the norminal voltages while in powersave with exprimental powersave enabled */
	/** 
     * pmu file name
     */
    #define PMU_JSON_CONFIG_FILE            "/pmu.json"         /** @brief defines json config file name */
    #define PMU_CHARGE_LOG_FILENAME         "/pmu_charge.csv"   /** @brief defines csv logfile while charging when pmu logging is enabled */
    #define PMU_DISCHARGE_LOG_FILENAME      "/pmu_discharge.csv"/** @brief defines csv logfile while discharging when pmu logging is enabled */
    /**
     * @brief pmu config structure
     */
    typedef struct {
        int32_t designed_battery_cap = 300;
        int32_t silence_wakeup_interval = SILENCEWAKEINTERVAL;
        int32_t silence_wakeup_interval_vbplug = SILENCEWAKEINTERVAL_PLUG;
        int32_t normal_voltage = NORMALVOLTAGE;
        int32_t normal_power_save_voltage = NORMALPOWERSAVEVOLTAGE;
        int32_t experimental_normal_voltage = EXPERIMENTALNORMALVOLTAGE;
        int32_t experimental_power_save_voltage = EXPERIMENTALPOWERSAVEVOLTAGE;
        bool high_charging_target_voltage = false;
        bool compute_percent = false;
        bool experimental_power_save = false;
        bool silence_wakeup = true;
        bool pmu_logging = false;
    } pmu_config_t;
    /**
     * @brief setup pmu: axp202
     */
    void pmu_setup( void );
    /**
     * @brief pmu loop routine, call from powermgm. not for user use
     */
    void pmu_loop( void );
    /**
     * @brief get the charge of battery in percent
     * 
     * @return  charge in percent or -1 if unknown
     */
    int32_t pmu_get_battery_percent( void );
    /**
     * @brief shutdown the axp202
     */
    void pmu_shutdown( void );
    /**
     * @brief set the axp202 in standby
     */
    void pmu_standby( void );
    /**
     * @brief wakeup to axp202
     */
    void pmu_wakeup( void );
    /**
     * @brief save the config structure to SPIFFS
     */
    void pmu_save_config( void );
    /**
     * @brief read the config structure from SPIFFS
     */
    void pmu_read_config( void );
    /**
     * @brief read the config for calculated mAh based on the axp202 coloumb counter
     */
    bool pmu_get_calculated_percent( void );
    /**
     * @brief read the config for experimental power save
     * 
     * @return  true means enable, false means disable
     */
    bool pmu_get_experimental_power_save( void );
    /**
     * @brief set the config to use calculated mAh
     * 
     * @param   value   true enable calculated percent, false use AXP202 percent
     */
    void pmu_set_calculated_percent( bool value );
    /**
     * @brief set experimental power save
     * 
     * @param   value   true enable experiemental power save settings
     */
    void pmu_set_experimental_power_save( bool value );
    /**
     * @brief get the designed battery capacity in mAh
     * 
     * @return  capacity in mAh
     */
    int32_t pmu_get_designed_battery_cap( void );
    /**
     * @brief   get the current silence wakeup configuration,
     *          the time can configure in pmu.json
     * 
     * @return  true means enable, false means disable
     */
    bool pmu_get_silence_wakeup( void );
    /**
     * @brief set the current silence wakeup configureation
     * 
     * @param   value   true means enable, false means disable
     */
    void pmu_set_silence_wakeup( bool value );
    /**
     * @brief get the current battery voltage in mV
     * 
     * @return  voltage in mV
     */ 
    float pmu_get_battery_voltage( void );
    /**
     * @brief   get the batterty charging current in mA
     * 
     * @return  current in mA
     */
    float pmu_get_battery_charge_current( void );
    /**
     * @brief   get the battery discharging current in mA
     * 
     * @return  discharging current in mA
     */
    float pmu_get_battery_discharge_current( void );
    /**
     * @brief   get the VBUS voltage in mV
     * 
     * @return  VBUS voltage in mV
     */
    float pmu_get_vbus_voltage( void );
    /**
     * @brief   get battery capacity in mAh
     * 
     * @return  capacity in mAh
     */
    float pmu_get_coulumb_data( void );
    /**
     * @brief   get the charging state
     * 
     * @return  true means charging, false means no charging
     */
    bool pmu_is_charging( void );
    /**
     * @brief   get the vbus plug state
     * 
     * @return  true means plugged, false means not plugged
     */
    bool pmu_is_vbus_plug( void );
    /**
     * @brief get the high charging voltage config
     * 
     * @return true means enabled, false means disabled
     */
    bool pmu_get_high_charging_target_voltage( void );
    /**
     * @brief set the high charging voltage config
     * 
     * @param   enable  true means enable, false means disable
     */
    void pmu_set_high_charging_target_voltage( bool enable );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event           possible values: PMUCTL_STATUS
     * @param   callback_func   pointer to the callback function
     * @param   id              program id
     * 
     * @return  true if success, false if failed
     */
    bool pmu_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     *  @brief  set normal voltage (3.3V) for update/flashing
     */
    void pmu_set_safe_voltage_for_update( void );

#endif // _PMU_H