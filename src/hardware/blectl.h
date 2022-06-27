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
#ifndef _BLECTL_H
    #define _BLECTL_H

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
    #else
        #include "NimBLEDevice.h"
    #endif

    #include "callback.h"
    #include "hardware/config/blectlconfig.h"

    #define BLECTL_SCAN_TIME             30
    /**
     * connection state
     */
    #define BLECTL_CONNECT               _BV(0)         /** @brief event mask for blectl connect to an client */
    #define BLECTL_DISCONNECT            _BV(1)         /** @brief event mask for blectl disconnect */
    #define BLECTL_AUTHWAIT              _BV(2)         /** @brief event mask for blectl wait for auth to get connect */
    /**
     * power state
     */
    #define BLECTL_STANDBY               _BV(3)         /** @brief event mask for blectl standby */
    #define BLECTL_ON                    _BV(4)         /** @brief event mask for blectl on */
    #define BLECTL_OFF                   _BV(5)         /** @brief event mask for blectl off */
    /**
     * pairing state
     */
    #define BLECTL_PIN_AUTH              _BV(6)         /** @brief event mask for blectl for pin auth, callback arg is (uint32*) */
    #define BLECTL_PAIRING               _BV(7)         /** @brief event mask for blectl pairing requested */
    #define BLECTL_PAIRING_SUCCESS       _BV(8)         /** @brief event mask for blectl pairing success */
    #define BLECTL_PAIRING_ABORT         _BV(9)         /** @brief event mask for blectl pairing abort */
    /**
     * config updates
     */
    #define BLECTL_CONFIG_UPDATE         _BV(10)        /** @brief event mask for blectl config update */
    /**
     * @brief ble setup function
     */
    void blectl_setup( void );
    /**
     * @brief trigger a blectl managemt event
     * 
     * @param   bits    event to trigger
     */
    void blectl_set_event( EventBits_t bits );
    /**
     * @brief clear a blectl managemt event
     * 
     * @param   bits    event to clear
     */
    void blectl_clear_event( EventBits_t bits );
    /**
     * @brief get a blectl managemt event state
     * 
     * @param   bits    event state, example: POWERMGM_STANDBY to evaluate if the system in standby
     */
    bool blectl_get_event( EventBits_t bits );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event  possible values:     BLECTL_CONNECT,
     *                                      BLECTL_DISCONNECT,
     *                                      BLECTL_STANDBY,
     *                                      BLECTL_ON,
     *                                      BLECTL_OFF,       
     *                                      BLECTL_ACTIVE,    
     *                                      BLECTL_MSG,
     *                                      BLECTL_PIN_AUTH,
     *                                      BLECTL_PAIRING,
     *                                      BLECTL_PAIRING_SUCCESS,
     *                                      BLECTL_PAIRING_ABORT
     * @param   blectl_event_cb     pointer to the callback function
     * @param   id                  pointer to an string
     */
    bool blectl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief enable blueetooth on standby
     * 
     * @param   enable_on_standby   true means enabled, false means disabled 
     */
    void blectl_set_enable_on_standby( bool enable_on_standby );
    /**
     * @brief disable blueetooth only when disconnected
     * 
     * @param   disable_only_disconnected   true means enabled, false means disabled 
     */
    void blectl_set_disable_only_disconnected( bool disable_only_disconnected );
    /**
     * @brief wakeup on notification
     * 
     * @param   wakeup_on_notification   true means enabled, false means disabled 
     */
    void blectl_set_wakeup_on_notification( bool wakeup_on_notification );
    /**
     * @brief enable show notification
     * 
     * @param   show_notification   true means enabled, false means disabled 
     */
    void blectl_set_show_notification( bool show_notification );
    /**
     * @brief enable vibe notification
     * 
     * @param   vibe_notification   true means enabled, false means disabled 
     */
    void blectl_set_vibe_notification( bool vibe_notification );
    /**
     * @brief enable sound notification
     * 
     * @param   sound_notification   true means enabled, false means disabled 
     */
    void blectl_set_sound_notification( bool sound_notification );
    /**
     * @brief enable media notification
     * 
     * @param   media_notification   true means enabled, false means disabled 
     */
    void blectl_set_media_notification( bool media_notification );
    /**
     * @brief enable advertising
     * 
     * @param   advertising true means enabled, false means disabled
     */
    void blectl_set_advertising( bool advertising );
    /**
     * @brief get the current enable_on_standby config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_enable_on_standby( void );
    /**
     * @brief get the current disable_only_disconnected config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_disable_only_disconnected( void );
    /**
     * @brief get wakeup on notification
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_wakeup_on_notification( void );
    /**
     * @brief get the current show notification config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_show_notification( void );
    /**
     * @brief get the current vibe notification config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_vibe_notification( void );
    /**
     * @brief get the current sound notification config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_sound_notification( void );
    /**
     * @brief get the current media notification config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_media_notification( void );
    /**
     * @brief get the current advertising config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_advertising( void );
    /**
     * @brief get the current custom audio notifications config
     * 
     * @return  the array of custom audio notifications
     */
    blectl_custom_audio* blectl_get_custom_audio_notifications( void );
    /**
     * @brief store the current configuration to SPIFFS
     */
    void blectl_save_config( void );
    /**
     * @brief read the configuration from SPIFFS
     */
    void blectl_read_config( void );
    /**
     * @brief set the transmission power
     * 
     * @param   txpower power from 0..4, from -12db to 0db in 3db steps
     */
    void blectl_set_txpower( int32_t txpower );
    /**
     * @brief get the current transmission power
     * 
     * @return  power from 0..4, from -12db to 0db in 3db steps
     */
    int32_t blectl_get_txpower( void );
    /**
     * @brief enable the bluettoth stack
     */
    void blectl_on( void );
    /**
     * @brief disable the bluetooth stack
     */
    void blectl_off( void );
    /**
     * @brief get the current enable config
     * 
     * @return true if bl enabled, false if bl disabled
     */
    bool blectl_get_autoon( void );
    /**
     * @brief set the current bl enable config
     * 
     * @param enable    true if enabled, false if disable
     */
    void blectl_set_autoon( bool autoon );

#ifndef NATIVE_64BIT
    /**
     * @brief get the raw BLE Server
     */
    NimBLEServer *blectl_get_ble_server( void );
    /**
     * @brief get the raw BLE Advertising
     */
    NimBLEAdvertising *blectl_get_ble_advertising( void );
#endif

#endif // _BLECTL_H