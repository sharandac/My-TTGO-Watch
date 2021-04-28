/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#ifndef _URI_LOAD__H
    #define _URI_LOAD__H

    typedef struct {
        char *filename;         /** @brief pointer to the downloaded filename */
        char *uri;              /** @brief pointer to the downloaded filename */
        uint32_t size;          /** @brief filesize in bytes */
        uint64_t timestamp;     /** @brief download timestamp */
        uint8_t *data;          /** @brief pointer to the downloaded data */
    } uri_load_dsc_t;
    
    /**
     * @brief doenload a file from a webserver into ram
     * 
     * @param   url requested url to get a file from
     * 
     * @return  uri_load_dsc structure
     */
    uri_load_dsc_t *uri_load_to_ram( const char *uri );
    /**
     * @brief delete the complete uri_load_dsc structure and free all allocated memory
     * 
     * @param uri_load pointer to a uri_load_dsc structure
     */
    void uri_load_free_all( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete the filename from the uri_load_dsc structure and free their allocated memory
     * 
     * @param uri_load pointer to a uri_load_dsc structure
     */
    void uri_load_free_name( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete the url from the uri_load_dsc structure and free their allocated memory
     * 
     * @param uri_load pointer to a uri_load_dsc structure
     */
    void uri_load_free_url( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete the data from the uri_load_dsc structure and free their allocated memory
     * 
     * @param uri_load pointer to a uri_load_dsc structure
     */
    void uri_load_free_data( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete the uri_load_dsc structure and leave data in allocated
     * 
     * @param uri_load pointer to a uri_load_dsc structure
     */
    void uri_load_free_without_data( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete only the allocated uri_load_sdc structure and leave the name/data memory allocated
     * for later use
     * 
     * @param uri_load pointer to a uri_load_dsc structure
     */
    void uri_load_free_dsc( uri_load_dsc_t *uri_load_dsc );

#endif // _URI_LOAD__H
