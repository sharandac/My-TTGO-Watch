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

    #include "TTGO.h"
    
    #define BMACTL_DOUBLECLICK         _BV(0)
    #define BMACTL_STEPCOUNTER         _BV(1)
    #define BMACTL_TILT                _BV(2)

    typedef void ( * BMA_CALLBACK_FUNC ) ( EventBits_t event, const char *msg );

    typedef struct {
        EventBits_t event;
        BMA_CALLBACK_FUNC event_cb;
    } bma_event_cb_t;

    #define BMA_COFIG_FILE          "/bma.cfg"
    #define BMA_JSON_COFIG_FILE     "/bma.json"

    typedef struct {
        bool enable=true;
    } bma_config_t;

    enum {  
        BMA_STEPCOUNTER,
        BMA_DOUBLECLICK,
        BMA_TILT,
        BMA_CONFIG_NUM
    };

    /*
     * @brief setup bma activity measurement
     */
    void bma_setup( void );
    /*
     * @brief loop function for activity measurement
     */
    void bma_loop( void );
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
     * @brief save the config structure to SPIFFS
     */
    void bma_save_config( void );
    /*
     * @brief read the config structure from SPIFFS
     */
    void bma_read_config( void );
    /*
     * @brief get config
     * 
     * @param   config     configitem: BMA_STEPCOUNTER, BMA_DOUBLECLICK or BMA_CONFIG_NUM
     */
    bool bma_get_config( int config );
    /*
     * @brief set config
     * 
     * @param   config     configitem: BMA_STEPCOUNTER, BMA_DOUBLECLICK or BMA_CONFIG_NUM
     * @param   bool    true or false
     */
    void bma_set_config( int config, bool enable );
    /*
     * @brief   rotate bma axis
     * 
     * @param   rotation on degree
     */
    void bma_set_rotate_tilt( uint32_t rotation );
    /*
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event   possible values: BMACTL_DOUBLECLICK, BMACTL_STEPCOUNTER and BMACTL_TILT
     * @param   rtc_event_cb   pointer to the callback function 
     */
    void bma_register_cb( EventBits_t event, BMA_CALLBACK_FUNC bma_event_cb );

#endif // _BMA_H