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
#include "watchface_config.h"

watchface_config_t::watchface_config_t() : BaseJsonConfig( WATCHFACE_JSON_COFIG_FILE ) {}

bool watchface_config_t::onSave(JsonDocument& doc) {
    doc["watchface_enable"] = watchface_enable;
    doc["watchface_antialias"] = watchface_antialias;
    doc["watchface_theme_url"] = watchface_theme_url;
    doc["watchface_show_notifications"] = watchface_show_notifications;
    return true;
}

bool watchface_config_t::onLoad(JsonDocument& doc) {
    watchface_enable = doc["watchface_enable"] | false;
    watchface_antialias = doc["watchface_antialias"] | true;
    watchface_show_notifications = doc["watchface_show_notifications"] | true;
    /**
     * force use own theme url on alpha/beta tests
     */
    // watchface_theme_url = doc["watchface_theme_url"] | WATCHFACE_THEME_URL;
    watchface_theme_url = WATCHFACE_THEME_URL;
    return true;
}

bool watchface_config_t::onDefault( void ) {
    watchface_enable = false;
    watchface_antialias = true;
    watchface_show_notifications = true;
    watchface_theme_url = WATCHFACE_THEME_URL;
    return true;
}