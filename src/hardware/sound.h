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

    #define SOUND_JSON_CONFIG_FILE    "/sound.json"

    typedef struct {
        bool enable = true;
        uint8_t volume = 50;
    } sound_config_t;

    /*
    * @brief put sound output to standby (disable)
    */
    void sound_standby( void );

    /*
    * @brief wakeup sound output
    */
    void sound_wakeup( void );
    
    /**
     * @brief enable or disable the power output for AXP202_LDO3
     * 
     * @param enable = true sets the AXP202_LDO3 power output to high false to low
     */
    void sound_set_enabled( bool enabled = true );

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
    /*
     * @brief get the sound enabled value
     * 
     * @return true if sound is enabled, false if not
     */
    bool sound_get_enabled_config( void );
    /*
     * @brief   set the sound enabled configuration
     * 
     * @param   enable    true = enabled, false = disabled
     */
    void sound_set_enabled_config( bool enable );
    /*
     * @brief get the current volume configuration
     * 
     * @return volume value between 0-100
     */
    uint8_t sound_get_volume_config( void );
    /*
     * @brief set the current volume configuration
     * 
     * @param volume from 0-100
     */
    void sound_set_volume_config( uint8_t volume );



#endif // _SOUND_H