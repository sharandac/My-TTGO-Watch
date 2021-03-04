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
        uint64_t current_millis = millis();
        if (force || last_value != value) {
            set(value);
        }
        if (force ||
            last_millis - current_millis > timeout_millis) {
            // Time to notify
            bool ret = notify(value);
            if (ret) {
                // new value was published
                last_millis = current_millis;
                last_value = value;
            }
        }
    }
    void setTimeout(uint64_t timeout){ timeout_millis = timeout; }
    protected:
    BleUpdater(uint64_t timeout): timeout_millis(timeout) {}
    virtual void set(T value) { /* Nothing by default */ }
    virtual bool notify(T value) = 0;
    T last_value;
    uint64_t last_millis = 0;
    uint64_t timeout_millis = 0;
};

