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
#include "mail_config.h"

mail_config_t::mail_config_t() : BaseJsonConfig( MAIL_JSON_COFIG_FILE ) {}

bool mail_config_t::onSave(JsonDocument& doc) {
    doc["imap_server"] = imap_server;
    doc["imap_port"] = imap_port;
    doc["use_ssl"] = use_ssl;
    doc["inbox_folder"] = inbox_folder;
    doc["max_msg"] = max_msg;
    doc["max_msg_size"] = max_msg_size;
    doc["username"] = username;
    doc["password"] = password;

    return true;
}

bool mail_config_t::onLoad(JsonDocument& doc) {
    strncpy( imap_server, doc["imap_server"], sizeof( imap_server ) );
    imap_port = doc["imap_port"] | 993;
    use_ssl = doc["use_ssl"] | true;
    strncpy( inbox_folder, doc["inbox_folder"], sizeof( inbox_folder ) );
    max_msg = doc["max_msg"] | 5;
    max_msg_size = doc["max_msg_size"] | 512;
    strncpy( username, doc["username"], sizeof( username ) );
    strncpy( password, doc["password"], sizeof( password ) );
    return true;
}

bool mail_config_t::onDefault( void ) {
    return true;
}