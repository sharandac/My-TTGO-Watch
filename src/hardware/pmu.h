#ifndef _PMU_H
    #define _PMU_H

    #define PMU_EVENT_AXP_INT       _BV(0)

    #define PMU_BATTERY_CAP         380

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