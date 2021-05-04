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
#include "watchface_theme_config.h"

watchface_theme_config_t::watchface_theme_config_t() : BaseJsonConfig( WATCHFACE_THEME_JSON_COFIG_FILE ) {}

bool watchface_theme_config_t::onSave(JsonDocument& doc) {
    doc["hour"]["enable"] = dial.hour.enable;
    doc["hour"]["x_offset"] = dial.hour.x_offset;
    doc["hour"]["y_offset"] = dial.hour.y_offset;
    doc["min"]["enable"] = dial.min.enable;
    doc["min"]["x_offset"] = dial.min.x_offset;
    doc["min"]["y_offset"] = dial.min.y_offset;
    doc["sec"]["enable"] = dial.sec.enable;;
    doc["sec"]["x_offset"] = dial.sec.y_offset;
    doc["sec"]["y_offset"] = dial.sec.y_offset;

    doc["hour_shadow"]["enable"] = dial.hour_shadow.enable;
    doc["hour_shadow"]["x_offset"] = dial.hour_shadow.x_offset;;
    doc["hour_shadow"]["y_offset"] = dial.hour_shadow.y_offset;;
    doc["min_shadow"]["enable"] = dial.min_shadow.enable;
    doc["min_shadow"]["x_offset"] = dial.min_shadow.x_offset;;
    doc["min_shadow"]["y_offset"] = dial.min_shadow.y_offset;;
    doc["sec_shadow"]["enable"] = dial.sec_shadow.enable;
    doc["sec_shadow"]["x_offset"] = dial.sec_shadow.x_offset;;
    doc["sec_shadow"]["y_offset"] = dial.sec_shadow.y_offset;
    return true;
}

bool watchface_theme_config_t::onLoad(JsonDocument& doc) {
    dial.hour.enable = doc["hour"]["enable"] | true;
    dial.hour.x_offset = doc["hour"]["x_offset"] | 0;
    dial.hour.y_offset = doc["hour"]["y_offset"] | 0;
    dial.min.enable = doc["min"]["enable"] | true;
    dial.min.x_offset = doc["min"]["x_offset"] | 0;
    dial.min.y_offset = doc["min"]["y_offset"] | 0;
    dial.sec.enable = doc["sec"]["enable"] | true;
    dial.sec.x_offset = doc["sec"]["x_offset"] | 0;
    dial.sec.y_offset = doc["sec"]["y_offset"] | 0;

    dial.hour_shadow.enable = doc["hour_shadow"]["enable"] | true;
    dial.hour_shadow.x_offset = doc["hour_shadow"]["x_offset"] | 5;
    dial.hour_shadow.y_offset = doc["hour_shadow"]["y_offset"] | 5;
    dial.min_shadow.enable = doc["min_shadow"]["enable"] | true;
    dial.min_shadow.x_offset = doc["min_shadow"]["x_offset"] | 5;
    dial.min_shadow.y_offset = doc["min_shadow"]["y_offset"] | 5;
    dial.sec_shadow.enable = doc["sec_shadow"]["enable"] | true;
    dial.sec_shadow.x_offset = doc["sec_shadow"]["x_offset"] | 5;
    dial.sec_shadow.y_offset = doc["sec_shadow"]["y_offset"] | 5;
    return true;
}

bool watchface_theme_config_t::onDefault( void ) {
    dial.hour.enable = true;
    dial.hour.x_offset = 0;
    dial.hour.y_offset = 0;
    dial.min.enable = true;
    dial.min.x_offset = 0;
    dial.min.y_offset = 0;
    dial.sec.enable = true;
    dial.sec.x_offset = 0;
    dial.sec.y_offset = 0;

    dial.hour_shadow.enable = true;
    dial.hour_shadow.x_offset = 5;
    dial.hour_shadow.y_offset = 5;
    dial.min_shadow.enable = true;
    dial.min_shadow.x_offset = 5;
    dial.min_shadow.y_offset = 5;
    dial.sec_shadow.enable = true;
    dial.sec_shadow.x_offset = 5;
    dial.sec_shadow.y_offset = 5;
    return true;
}