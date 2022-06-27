/****************************************************************************
 *   Copyright  2021  Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
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
#include "config.h"
#include <time.h>

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #else // NEW_HARDWARE_TAG
    #endif
#endif

/**
 * @brief Timeout based updater.
 */
template <class T>
class BleUpdater {
    public:
    /**
     * @brief Update of value.
     * 
     * @param value the new value
     * @param force force the notification of the new value
     */
    void update(T value, bool force = false) {
        if ( !blectl_get_event( BLECTL_CONNECT ) )
            // BLE inactive, nothing to update
            return;
        time_t current_time;
        time(&current_time);
        log_d("Updating: last_value=%d value=%d last_time=%lu current_time=%lu", last_value, value, last_time, current_time);
        if (force || last_value != value) {
            set(value);
        }
        if (force ||
            current_time - last_time  > timeout) {
            // Time to notify
            bool ret = notify(value);
            if (ret) {
                // new value was published
                last_time = current_time;
                last_value = value;
            }
        }
    }
    /**
     * @brief Change the timeout.
     * 
     * @param timeout new timeout value, in seconds
     */
    void setTimeout(time_t timeout){ this->timeout = timeout; }
    protected:
    /**
     * @brief Constructor
     * 
     * @param timeout the timeout to respect, in seconds
     */
    BleUpdater(time_t timeout): timeout(timeout) {}
    /**
     * @brief Set the new value
     * 
     * Used to inform subclass of new value.
     */
    virtual void set(T value) { /* Nothing by default */ }
    /**
     * @brief Notify the value in BLE channels
     * 
     * @param value the new value
     */
    virtual bool notify(T value) = 0;
    /**
     * @brief the previous notified value
     */
    T last_value;
    /**
     * @brief the time of the previous notification
     */
    time_t last_time = 0;
    /**
     * @brief the current timeout
     */
    time_t timeout = 0;
};

