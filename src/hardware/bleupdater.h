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

/*
 * @brief Timeout based updater.
 */
template <class T>
class BleUpdater {
    public:
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
    void setTimeout(time_t timeout){ this->timeout = timeout; }
    protected:
    BleUpdater(time_t timeout): timeout(timeout) {}
    virtual void set(T value) { /* Nothing by default */ }
    virtual bool notify(T value) = 0;
    T last_value;
    time_t last_time = 0;
    time_t timeout = 0;
};

