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

    void bma_setup( TTGOClass *ttgo );
    void bma_loop( TTGOClass *ttgo );
    void bma_standby( void );
    void bma_wakeup( void );
    void bma_reload_settings( void );
    void bma_save_config( void );
    void bma_read_config( void );
    bool bma_get_config( int config );
    void bma_set_config( int config, bool enable );

#endif // _BMA_H