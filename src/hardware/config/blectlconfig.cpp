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
#include "blectlconfig.h"

blectl_config_t::blectl_config_t() : BaseJsonConfig(BLECTL_JSON_COFIG_FILE) {
}

bool blectl_config_t::onSave(JsonDocument& doc) {
    doc["autoon"] = autoon;
    doc["advertising"] = advertising;
    doc["enable_on_standby"] = enable_on_standby;
    doc["disable_only_disconnected"] = disable_only_disconnected;
    doc["tx_power"] = txpower;
    doc["show_notification"] = show_notification;

    return true;
}

bool blectl_config_t::onLoad(JsonDocument& doc) {
    autoon = doc["autoon"] | true;
    advertising = doc["advertising"] | true;
    enable_on_standby = doc["enable_on_standby"] | false;
    disable_only_disconnected = doc["disable_only_disconnected"] | false;
    txpower = doc["tx_power"] | 1;
    show_notification = doc["show_notification"] | true;
  
    return true;
}

bool blectl_config_t::onDefault( void ) {
    return true;
}