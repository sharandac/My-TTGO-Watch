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
#ifndef _WIFIMON_APP_MAIN_H
    #define _WIFIMON_APP_MAIN_H

    /**
     * @brief ieee80211 mac header
     */
    typedef struct {
        unsigned frame_ctrl:16;
        unsigned duration_id:16;
        uint8_t addr1[6]; /* receiver address */
        uint8_t addr2[6]; /* sender address */
        uint8_t addr3[6]; /* filtering address */
        unsigned sequence_ctrl:16;
        uint8_t addr4[6]; /* optional */
    } wifi_ieee80211_mac_hdr_t;

    /**
     * @brief ieee80211 packet
     */
    typedef struct {
        wifi_ieee80211_mac_hdr_t hdr;
        uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
    } wifi_ieee80211_packet_t;

    void wifimon_app_main_setup( uint32_t tile_num );

#endif // _WIFIMON_APP_MAIN_H
