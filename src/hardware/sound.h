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
#ifndef _SOUND_H
    #define _SOUND_H

    #include "TTGO.h"

    #define SOUND_CONFIG_FILE         "/sound.cfg"
    #define SOUND_JSON_CONFIG_FILE    "/sound.json"

    typedef struct {
        bool enable = true;
    } sound_config_t;

    /*
     * @brief play mp3 file from SPIFFS by path/filename
     * 
     * @param   filename    the SPIFFS path to the file to be played
     */
    void sound_play_spiffs_mp3( const char *filename );

    /*
     * @brief play wave sound from PROGMEM
     * 
     * To transform an file to *data use: `xxd -i inout.wav > output.c`
     * 
     * @param   data    data from PROGMEM as array
     * @param   len     data array length
     * 
     */
    void sound_play_progmem_wav( const void *data, uint32_t len );

    /*
     * @brief setup sound
     * 
     */
    void sound_setup( void );
    /*
     * @brief sound loop
     * 
     */
    void sound_loop( void );
    /*
     * @brief save config for sound to spiffs
     */
    void sound_save_config( void );
    /*
     * @brief read config for sound from spiffs
     */
    void sound_read_config( void );


#endif // _SOUND_H