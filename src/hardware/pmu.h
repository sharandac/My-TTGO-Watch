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

    #define PMU_EVENT_AXP_INT       _BV(0)

    #define PMU_BATTERY_CAP         300

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
    uint32_t pmu_get_byttery_percent( TTGOClass *ttgo );

#endif // _PMU_H