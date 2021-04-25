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
#ifndef _HTTP_DOWNLOAD_H
    #define _HTTP_DOWNLOAD_H

    typedef struct {
        char *filename;         /** @brief pointer to the downloaded filename */
        char *url;              /** @brief pointer to the downloaded filename */
        uint32_t size;          /** @brief filesize in bytes */
        uint64_t timestamp;     /** @brief download timestamp */
        uint8_t *data;          /** @brief pointer to the downloaded data */
    } http_download_dsc_t;
    
    /**
     * @brief doenload a file from a webserver into ram
     * 
     * @param   url requested url to get a file from
     * 
     * @return  http_download_dsc structure
     */
    http_download_dsc_t *http_download_to_ram( const char *url );
    /**
     * @brief delete the complete http_download_dsc structure and free all allocated memory
     * 
     * @param http_download pointer to a http_download_dsc structure
     */
    void http_download_free_all( http_download_dsc_t *http_download );
    /**
     * @brief delete the filename from the http_download_dsc structure and free their allocated memory
     * 
     * @param http_download pointer to a http_download_dsc structure
     */
    void http_download_free_name( http_download_dsc_t *http_download );
    /**
     * @brief delete the url from the http_download_dsc structure and free their allocated memory
     * 
     * @param http_download pointer to a http_download_dsc structure
     */
    void http_download_free_url( http_download_dsc_t *http_download );
    /**
     * @brief delete the data from the http_download_dsc structure and free their allocated memory
     * 
     * @param http_download pointer to a http_download_dsc structure
     */
    void http_download_free_data( http_download_dsc_t *http_download );
    /**
     * @brief delete the http_download_dsc structure and leave data in allocated
     * 
     * @param http_download pointer to a http_download_dsc structure
     */
    void http_download_free_without_data( http_download_dsc_t *http_download );
    /**
     * @brief delete only the allocated http_download_sdc structure and leave the name/data memory allocated
     * for later use
     * 
     * @param http_download pointer to a http_download_dsc structure
     */
    void http_download_free_dsc( http_download_dsc_t *http_download );

#endif // _HTTP_DOWNLOAD_H
