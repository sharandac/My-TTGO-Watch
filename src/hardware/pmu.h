#ifndef _PMU_H
    #define _PMU_H

    #define PMU_EVENT_AXP_INT       _BV(0)

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

#endif // _PMU_H