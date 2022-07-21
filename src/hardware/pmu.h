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

    #include "callback.h"
    #include "hardware/config/pmuconfig.h"

	/**
     * PMU events mask
     */
    #define PMUCTL_STATUS                   _BV(0)              /** @brief event mask for pmuctl battery status update, callback arg is (int32_t*) */
    #define PMUCTL_SHORT_PRESS              _BV(1)              /** @brief event mask for pmuctl short press, no callback arg */
    #define PMUCTL_LONG_PRESS               _BV(2)              /** @brief event mask for pmuctl long press, nocallback arg */
    #define PMUCTL_TIMER_TIMEOUT            _BV(3)              /** @brief event mask for pmuctl timer timeout, no callback arg */
    #define PMUCTL_UP_PRESS                 _BV(4)              /** @brief event mask for pmuctl up press, no callback arg */
    #define PMUCTL_DOWN                     _BV(5)              /** @brief event mask for pmuctl down press, no callback arg */
    #define PMUCTL_CALIBRATION_START        _BV(6)              /** @brief event mask for pmuctl calibration start event */
    #define PMUCTL_CALIBRATION_UPDATE       _BV(7)              /** @brief event mask for pmuctl calibration new data event */
    /**
     * PMU status mask
     */
    #define PMUCTL_STATUS_PERCENT           0xFF                /** @brief pmu status percent mask */
    #define PMUCTL_STATUS_PLUG              0x100               /** @brief pmu status plug flag mask */
    #define PMUCTL_STATUS_CHARGING          0x200               /** @brief pmu status charging flag mask */
    #define PMUCTL_STATUS_BATTERY           0x400               /** @brief pmu status battery flag mask */
	/** 
     * pmu logging file names
     */
    #define PMU_CHARGE_LOG_FILENAME         "/pmu_charge.csv"   /** @brief defines csv logfile while charging when pmu logging is enabled */
    #define PMU_DISCHARGE_LOG_FILENAME      "/pmu_discharge.csv"/** @brief defines csv logfile while discharging when pmu logging is enabled */
    /**
     * 
     */
    typedef struct {
        bool    run = false;
        bool    store = false;
        bool    charging = false;
        bool    VBUS = false;
        int64_t nextmillis = 0;
        float   batteryVoltage;
        float   minVoltage;
        float   maxVoltage;
        float   maxVoltageCharge;
        float   chargingVoltageOffset;
    } calibration_data_t;
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
     * @brief get the current pmu logging state
     * 
     * @return  true if logging enabled
     */ 
    bool pmu_get_logging( void );
    /**
     * @brief get the current pmu logging state
     * 
     * @return  true if logging enabled
     */ 
    void pmu_set_logging( bool logging );
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
    /**
     * @brief start battery voltage range calibration
     * 
     * @param start_calibration     true to start, only works when store is false
     * @param store                 true to store values, only works when start_calibration is false
     * @return pointer to the current calibration data or NULL
     */
    calibration_data_t *pmu_battery_calibration_loop( bool start_calibration, bool store );

#endif // _PMU_H