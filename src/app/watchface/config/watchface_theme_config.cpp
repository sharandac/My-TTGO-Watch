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

bool watchface_theme_config_t::onSave(JsonDocument& doc ) {
    doc["dial"]["enable"] = dial.dial.enable;
    doc["dial"]["x_offset"] = dial.dial.x_offset;
    doc["dial"]["y_offset"] = dial.dial.y_offset;

    doc["hour"]["enable"] = dial.hour.enable;
    doc["hour"]["x_offset"] = dial.hour.x_offset;
    doc["hour"]["y_offset"] = dial.hour.y_offset;
    doc["min"]["enable"] = dial.min.enable;
    doc["min"]["smooth"] = dial.min.smooth;
    doc["min"]["x_offset"] = dial.min.x_offset;
    doc["min"]["y_offset"] = dial.min.y_offset;
    doc["sec"]["enable"] = dial.sec.enable;;
    doc["sec"]["x_offset"] = dial.sec.y_offset;
    doc["sec"]["y_offset"] = dial.sec.y_offset;

    doc["hour_shadow"]["enable"] = dial.hour_shadow.enable;
    doc["hour_shadow"]["x_offset"] = dial.hour_shadow.x_offset;
    doc["hour_shadow"]["y_offset"] = dial.hour_shadow.y_offset;
    doc["min_shadow"]["enable"] = dial.min_shadow.enable;
    doc["min_shadow"]["x_offset"] = dial.min_shadow.x_offset;
    doc["min_shadow"]["y_offset"] = dial.min_shadow.y_offset;
    doc["sec_shadow"]["enable"] = dial.sec_shadow.enable;
    doc["sec_shadow"]["x_offset"] = dial.sec_shadow.x_offset;
    doc["sec_shadow"]["y_offset"] = dial.sec_shadow.y_offset;

    for( int i = 0 ; i < WATCHFACE_LABEL_NUM ; i++ ) {
        doc["label"][i]["enable"] = dial.label[ i ].enable;
        doc["label"][i]["type"] = dial.label[ i ].type;
        doc["label"][i]["label"] = dial.label[ i ].label;
        doc["label"][i]["font_color"] = dial.label[ i ].font_color;
        doc["label"][i]["align"] = dial.label[ i ].align;
        doc["label"][i]["font_size"] = dial.label[ i ].font_size;
        doc["label"][i]["x_offset"] = dial.label[ i ].x_offset;
        doc["label"][i]["y_offset"] = dial.label[ i ].y_offset;
        doc["label"][i]["x_size"] = dial.label[ i ].x_size;
        doc["label"][i]["y_size"] = dial.label[ i ].y_size;
    }
    
    return true;
}

bool watchface_theme_config_t::onLoad(JsonDocument& doc) {
    dial.dial.enable = doc["dial"]["enable"] | true;
    dial.dial.x_offset = doc["dial"]["x_offset"] | 0;
    dial.dial.y_offset = doc["dial"]["y_offset"] | 0;

    dial.hour.enable = doc["hour"]["enable"] | true;
    dial.hour.x_offset = doc["hour"]["x_offset"] | 0;
    dial.hour.y_offset = doc["hour"]["y_offset"] | 0;
    dial.min.enable = doc["min"]["enable"] | true;
    dial.min.smooth = doc["min"]["smooth"] | true;
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

    for( int i = 0 ; i < WATCHFACE_LABEL_NUM ; i++ ) {
        dial.label[ i ].enable = doc["label"][i]["enable"] | false;
        strncpy( dial.label[ i ].type, doc["label"][i]["type"] | "text", sizeof( dial.label[ i ].type ) );
        strncpy( dial.label[ i ].label, doc["label"][i]["label"] | "" , sizeof( dial.label[ i ].label ) );
        strncpy( dial.label[ i ].align, doc["label"][i]["align"] | "center" , sizeof( dial.label[ i ].align ) );
        strncpy( dial.label[ i ].font_color, doc["label"][i]["font_color"] | "#ffffff" , sizeof( dial.label[ i ].font_color ) );
        dial.label[ i ].font_size = doc["label"][i]["font_size"] | 12;
        dial.label[ i ].x_offset = doc["label"][i]["x_offset"] | 0;
        dial.label[ i ].y_offset = doc["label"][i]["y_offset"] | 0;
        dial.label[ i ].x_size = doc["label"][i]["x_size"] | 0;
        dial.label[ i ].y_size = doc["label"][i]["y_size"] | 0;
    }
    return true;
}

bool watchface_theme_config_t::onDefault( void ) {
    /**
     * set default dial
     */
    dial.dial.enable = true;
    dial.dial.x_offset = 0;
    dial.dial.y_offset = 0;
    /**
     * set default index
     */
    dial.hour.enable = true;
    dial.hour.x_offset = 0;
    dial.hour.y_offset = 0;
    dial.min.enable = true;
    dial.min.smooth = true;
    dial.min.x_offset = 0;
    dial.min.y_offset = 0;
    dial.sec.enable = true;
    dial.sec.x_offset = 0;
    dial.sec.y_offset = 0;
    /**
     * set default shadow
     */
    dial.hour_shadow.enable = true;
    dial.hour_shadow.x_offset = 5;
    dial.hour_shadow.y_offset = 5;
    dial.min_shadow.enable = true;
    dial.min_shadow.x_offset = 5;
    dial.min_shadow.y_offset = 5;
    dial.sec_shadow.enable = true;
    dial.sec_shadow.x_offset = 5;
    dial.sec_shadow.y_offset = 5;
    /**
     * clear all labels
     */
    for( int i = 0 ; i < WATCHFACE_LABEL_NUM ; i++ ) {
        dial.label[ i ].enable = false;
        strncpy( dial.label[ i ].type, "text", sizeof( dial.label[ i ].type ) );
        strncpy( dial.label[ i ].label, "n/a" , sizeof( dial.label[ i ].label ) );
        strncpy( dial.label[ i ].font_color, "#000000" , sizeof( dial.label[ i ].font_color ) );
        strncpy( dial.label[ i ].align, "center" , sizeof( dial.label[ i ].align ) );
        dial.label[ i ].font_size = 12;
        dial.label[ i ].x_offset = 0;
        dial.label[ i ].y_offset = 0;
        dial.label[ i ].x_size = 0;
        dial.label[ i ].y_size = 0;
    }
    /**
     * setup default date label
     */
    dial.label[ 0 ].enable = true;
    strncpy( dial.label[ 0 ].type, "date", sizeof( dial.label[ 0 ].type ) );
    strncpy( dial.label[ 0 ].label, "%d.%b", sizeof( dial.label[ 0 ].label ) );
    strncpy( dial.label[ 0 ].font_color, "#000000", sizeof( dial.label[ 0 ].font_color ) );
    strncpy( dial.label[ 0 ].align, "center" , sizeof( dial.label[ 0 ].align ) );
    dial.label[ 0 ].font_size = 32;
    dial.label[ 0 ].x_offset = 64;
    dial.label[ 0 ].y_offset = 64;
    dial.label[ 0 ].x_size = 112;
    dial.label[ 0 ].y_size = 40;

    return true;
}