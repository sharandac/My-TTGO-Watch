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
#ifndef _BMA_H
    #define _BMA_H
    
    #define     BMA_EVENT_INT   _BV(0)

    typedef struct {
        bool enable=true;
    } bma_config_t;

    enum {  
        BMA_STEPCOUNTER,
        BMA_DOUBLECLICK,
        BMA_CONFIG_NUM
    };

    #define BMA_COFIG_FILE  "/bma.cfg"

    /*
     * @brief setup bma activity measurement
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void bma_setup( TTGOClass *ttgo );
    /*
     * @brief loop function for activity measurement
     */
    void bma_loop( TTGOClass *ttgo );
    /*
     * @brief put bma into standby, depending on ther config
     */
    void bma_standby( void );
    /*
     * @brief wakeup activity measurement
     */
    void bma_wakeup( void );
    /*
     * @brief reload config
     */
    void bma_reload_settings( void );
    /*
     * @ brief save the config structure to SPIFF
     */
    void bma_save_config( void );
    /*
     * @ brief read the config structure from SPIFF
     */
    void bma_read_config( void );
    /*
     * @brief get config
     * 
     * @param   config  configitem
     */
    bool bma_get_config( int config );
    /*
     * @brief set config
     * 
     * @param   config  configitem
     * @param   enable  true or false
     */
    void bma_set_config( int config, bool enable );

#endif // _BMA_H