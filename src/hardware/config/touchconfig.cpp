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
#include "touchconfig.h"

touch_config_t::touch_config_t() : BaseJsonConfig( TOUCH_JSON_CONFIG_FILE ) {
}

bool touch_config_t::onSave( JsonDocument& doc ) {
    doc["x_scale"] = x_scale;
    doc["y_scale"] = y_scale;

    return true;
}

bool touch_config_t::onLoad( JsonDocument& doc ) {
    x_scale = doc["x_scale"] | TOUCH_X_SCALE;
    y_scale = doc["y_scale"] | TOUCH_Y_SCALE;

    return true;
}

bool touch_config_t::onDefault( void ) {
    return true;
}