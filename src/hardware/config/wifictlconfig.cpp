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

#include "config.h"
#include "wifictlconfig.h"
#include "utils/alloc.h"
#include "utils/webserver/webserver.h"
#include "utils/ftpserver/ftpserver.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#endif

wifictl_config_t::wifictl_config_t() : BaseJsonConfig( WIFICTL_JSON_CONFIG_FILE ) {}

bool wifictl_config_t::onSave(JsonDocument& doc) {
    /*
     * save config structure into json file
     */
    doc["autoon"] = autoon;
    doc["hostname"] = hostname;
    doc["webserver"] = webserver;
    doc["ftpserver"] = ftpserver;
    doc["ftpuser"] = ftpuser;
    doc["ftppass"] = ftppass;

    doc["enable_on_standby"] = enable_on_standby;
    for ( int i = 0 ; i < NETWORKLIST_ENTRYS ; i++ ) {
        doc["networklist"][ i ]["ssid"] = networklist[ i ].ssid;
        doc["networklist"][ i ]["psk"] = networklist[ i ].password;
    }

    return true;
}

bool wifictl_config_t::onLoad(JsonDocument& doc) {
    /*
     * allocate networklist if needed
     */
    if ( networklist == NULL ) {
        networklist = ( wifictl_networklist* )CALLOC( sizeof( wifictl_networklist ) * NETWORKLIST_ENTRYS, 1 );
        if( !networklist ) {
            log_e("wifictl_networklist calloc faild");
            while(true);
        }
    }
    if ( networklist_tried == NULL ) {
        networklist_tried = ( wifictl_networklist* )CALLOC( sizeof( wifictl_networklist ) * NETWORKLIST_ENTRYS, 1 );
        if( !networklist_tried ) {
            log_e("wifictl_networklist_tried calloc faild");
            while(true);
        }
    }
    /*
     * clean networklist
     */
    for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
      networklist[ entry ].ssid[ 0 ] = '\0';
      networklist[ entry ].password[ 0 ] = '\0';
    }
    /*
     * read values from json
     */
    autoon = doc["autoon"] | true;
    enable_on_standby = doc["enable_on_standby"] | false;
    if ( doc["hostname"] ) {
        strncpy( hostname, doc["hostname"], sizeof( hostname ) );
    }

    webserver = doc["webserver"] | false;
    ftpserver = doc["ftpserver"] | false;
    if ( doc["ftpuser"] ) {
        strncpy( ftpuser, doc["ftpuser"], sizeof( ftpuser ) );
    }
    else {
        strncpy( ftpuser, FTPSERVER_USER, sizeof( ftpuser ) );
    }
    if ( doc["ftppass"] ) {
        strncpy( ftppass, doc["ftppass"], sizeof( ftppass ) );
    }
    else {
        strncpy( ftppass, FTPSERVER_PASSWORD, sizeof( ftppass ) );
    }

    for ( int i = 0 ; i < NETWORKLIST_ENTRYS ; i++ ) {
        if ( doc["networklist"][ i ]["ssid"] && doc["networklist"][ i ]["psk"] ) {
            strncpy( networklist[ i ].ssid    , doc["networklist"][ i ]["ssid"], sizeof( networklist[ i ].ssid ) );
            strncpy( networklist[ i ].password, doc["networklist"][ i ]["psk"], sizeof( networklist[ i ].password ) );
        }
    }

    return true;
}

bool wifictl_config_t::onDefault( void ) {
    /*
     * allocate networklist if needed
     */
    if ( networklist == NULL ) {
        networklist = ( wifictl_networklist* )CALLOC( sizeof( wifictl_networklist ) * NETWORKLIST_ENTRYS, 1 );
        if( !networklist ) {
            log_e("wifictl_networklist calloc faild");
            while(true);
        }
    }
    /*
     * clean networklist
     */
    for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
      networklist[ entry ].ssid[ 0 ] = '\0';
      networklist[ entry ].password[ 0 ] = '\0';
    }

    /*
     * read values from json
     */
    autoon = true;
    enable_on_standby = false;

    webserver = false;
    ftpserver = false;
    strncpy( ftpuser, FTPSERVER_USER, sizeof( ftpuser ) );
    strncpy( ftppass, FTPSERVER_PASSWORD, sizeof( ftppass ) );

    return( true );
}