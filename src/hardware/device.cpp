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
#include "device.h"
#include "config/deviceconfig.h"
#include "utils/logging.h"

device_config_t *device_config = NULL;

void device_setup( void ) {
    /**
     * get config
     */
    if( device_config )
        return;
    /**
     * alloc device config
     */
    device_config = new device_config_t();
    device_config->load();
    log_i("set device name to '%s'", device_config->device_name );
}

void device_set_name( const char * name ) {
    if( !device_config ) {
        device_config = new device_config_t();
        device_config->load();        
    }
    strncpy( device_config->device_name, name, sizeof( device_config->device_name ) );
    device_config->save();
}

const char *device_get_name( void ) {
    if( !device_config ) {
        device_config = new device_config_t();
        device_config->load();        
    }
    return( (const char*)device_config->device_name );
}