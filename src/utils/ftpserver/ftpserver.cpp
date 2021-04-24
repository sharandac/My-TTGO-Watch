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
#include <Arduino.h>
#include "config.h"
#include <TTGO.h>
#include <ESP-FTP-Server-Lib.h>
#include "hardware/powermgm.h"

FTPServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
bool ftpserver_powermgm_event_loop_cb( EventBits_t event, void *arg );

void ftpserver_start( const char *user, const char *pass ) {
    ftpSrv.addUser(user, pass);
    log_i("use ftp user/password: %s/%s", user, pass );
    ftpSrv.addFilesystem("SPIFFS", &SPIFFS);
    log_i("add SPIFFS to ftp server");
    #if defined( LILYGO_WATCH_HAS_SDCARD )
        ftpSrv.addFilesystem("SD", &SD);
        log_i("add SPIFFS to ftp server");
    #endif
    ftpSrv.begin();
    powermgm_register_loop_cb( POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, ftpserver_powermgm_event_loop_cb, "handle ftp" );
}

bool ftpserver_powermgm_event_loop_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_SILENCE_WAKEUP:
            ftpSrv.handle();
            break;
        case POWERMGM_WAKEUP:
            ftpSrv.handle();
            break;
    }
    return( true );
}