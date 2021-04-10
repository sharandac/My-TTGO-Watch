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
#include "soundconfig.h"

sound_config_t::sound_config_t() : BaseJsonConfig(SOUND_JSON_CONFIG_FILE) {
}

bool sound_config_t::onSave(JsonDocument& doc) {
    doc["enable"] = enable;
    doc["volume"] = volume;

    return true;
}

bool sound_config_t::onLoad(JsonDocument& doc) {
    enable = doc["enable"] | false;
    volume = doc["volume"] | 100;

    return true;
}

bool sound_config_t::onDefault( void ) {
    enable = false;
    volume = 100;

    return true;
}