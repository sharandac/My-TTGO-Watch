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
#ifndef _PMUCONFIG_H
    #define _PMUCONFIG_H

    #include "utils/basejsonconfig.h"

    #define PMU_JSON_CONFIG_FILE            "/pmu.json"         /** @brief defines json config file name */

	/**
     * Some default values, used below as well as in pmu.cpp during json reads
     */
    #define SILENCEWAKEINTERVAL             15                  /** @brief defines the silence wakeup interval in minutes */
	#define SILENCEWAKEINTERVAL_PLUG		5                   /** @brief defines the silence wakeup interval in minutes when plugged*/
    #define NORMALVOLTAGE                   3300                /** @brief defines the norminal voltages while working */
    #define NORMALPOWERSAVEVOLTAGE          3000                /** @brief defines the norminal voltages while in powersave */
    #define EXPERIMENTALNORMALVOLTAGE       3000                /** @brief defines the norminal voltages while working with exprimental powersave enabled */
    #define EXPERIMENTALPOWERSAVEVOLTAGE    2800                /** @brief defines the norminal voltages while in powersave with exprimental powersave enabled */
    /**
     * @brief pmu config structure
     */
    class pmu_config_t : public BaseJsonConfig {
        public:
        pmu_config_t();
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

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _PMUCONFIG_H