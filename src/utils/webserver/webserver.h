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

#ifndef _ASYNCWEBSERVER_H
    #define _ASYNCWEBSERVER_H
    
    #ifdef NATIVE_64BIT
        void asyncwebserver_start(void);
        void asyncwebserver_end(void);
    #else
        #include <FS.h>

        #define WEBSERVERPORT   80
        #define UPNPPORT        80

        #define DEV_NAME        "My-Watch" 
        #define DEV_INFO        "Watch based on ESP32 from Espressif Systems"

        /**
         *  @brief setup builtin webserver, call after first wifi-connection. otherwise esp32 will crash
         */
        void asyncwebserver_start(void);
        void asyncwebserver_end(void);
        /**
         *  @brief set a new filesystem for the SPIFFSEditor
         */
        void setFsEditorFilesystem(const fs::FS& fs);
    #endif


#endif // _ASYNCWEBSERVER_H