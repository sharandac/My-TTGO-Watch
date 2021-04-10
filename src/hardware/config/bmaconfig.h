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
#ifndef _BMA_CONFIG_H
    #define _BMA_CONFIG_H

    #include "utils/basejsonconfig.h"
    
    enum {  
        BMA_STEPCOUNTER,
        BMA_DOUBLECLICK,
        BMA_TILT,
        BMA_DAILY_STEPCOUNTER,
        BMA_CONFIG_NUM
    };

    #define BMA_JSON_COFIG_FILE         "/bma.json"         /** @brief defines json config file name */

    /**
     * @brief bma config structure
     */
    class bma_config_t : public BaseJsonConfig {
        public:
        bma_config_t();
        bool enable[BMA_CONFIG_NUM];

        inline bool get_config( int config ) {
            if ( config < BMA_CONFIG_NUM ) {
                return enable[ config ];
            }
            return false;
        }

        inline void bma_set_config( int config, bool ena ) {
            if ( config < BMA_CONFIG_NUM ) {
                enable[ config ] = ena;
            }
        }

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;


#endif // _BMA_CONFIG_H
