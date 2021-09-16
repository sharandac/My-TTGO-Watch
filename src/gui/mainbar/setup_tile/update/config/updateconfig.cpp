/****************************************************************************
 *   Aug 11 17:13:51 2020
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
#include "updateconfig.h"
#include "utils/alloc.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else

#endif

update_config_t::update_config_t() : BaseJsonConfig( UPDATE_JSON_COFIG_FILE ) {}

bool update_config_t::onSave(JsonDocument& doc) {
    doc["autosync"] = autosync;
    doc["autorestart"] = autorestart;
    if ( updateurl )
        doc["updateurl"] = updateurl;

    return true;
}

bool update_config_t::onLoad(JsonDocument& doc) {
    autosync = doc["autosync"] | true;
    autorestart = doc["autorestart"] | false;
    if ( !updateurl ) {
        updateurl = (char*)MALLOC( FIRMWARE_UPDATE_URL_LEN );
        if ( !updateurl ) {
            log_e("update url alloc failed");
            while( true ){};
        }
    }
    
    if ( doc["updateurl"] )
        strncpy( updateurl , doc["updateurl"], FIRMWARE_UPDATE_URL_LEN );
    else
        strncpy( updateurl , FIRMWARE_UPDATE_URL, FIRMWARE_UPDATE_URL_LEN );

    return true;
}

bool update_config_t::onDefault( void ) {
    autosync = true;
    autorestart = false;
    if ( !updateurl ) {
        updateurl = (char*)MALLOC( FIRMWARE_UPDATE_URL_LEN );
        if ( !updateurl ) {
            log_e("update url alloc failed");
            while( true ){};
        }
    }
    strncpy( updateurl , FIRMWARE_UPDATE_URL, FIRMWARE_UPDATE_URL_LEN );

    return true;
}