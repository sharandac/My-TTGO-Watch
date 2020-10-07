/****************************************************************************
 *   Sep 14 08:11:10 2020
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
#ifndef _CALLBACK_H
    #define _CALLBACK_H

    #include <stdint.h>

    /**
     * @brief typedef for the callback function call
     * 
     * @param event     event mask
     * @param arg       void pointer to an argument
     * 
     * @return          true if success or false if failed
     */
    typedef bool ( * CALLBACK_FUNC ) ( EventBits_t event, void *arg );

    /**
     * @brief callback table entry structure
     */
    typedef struct {
        EventBits_t event;                  /** @brief event mask */
        CALLBACK_FUNC callback_func;        /** @brief pointer to a callback function */
        const char *id;                     /** @brief id for the callback */
        uint64_t counter;                   /** @brief callback function call counter thair returned true */
    } callback_table_t;

    /**
     * @brief callback structure
     */
    typedef struct callback_t {
        uint32_t entrys;                    /** @brief count callback entrys */
        callback_table_t *table;            /** @brief pointer to an callback table */
        const char *name;                   /** @brief id for the callback structure */
        callback_t *next_callback_t;
    } callback_t;

    /**
     * @brief init the callback structure
     * 
     * @param   name        pointer to an string thats contains the name for the callback table
     * 
     * @return  pointer to a callback_t structure if success, NULL if failed
     */
    callback_t *callback_init( const char *name );
    /**
     * @brief   register an callback function
     * 
     * @param   callback        pointer to a callback_t structure
     * @param   event           event filter mask
     * @param   callback_func   pointer to a callbackfunc
     * @param   id              pointer to an string thats contains the id aka name for the callback function
     * 
     * @return  true if success, false if failed
     */
    bool callback_register( callback_t *callback, EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief   call all callback function thats match with the event filter mask
     * 
     * @param   callback        pointer to a callback_t structure
     * @param   event           event filter mask
     * @param   arg             argument for the called callback function
     * 
     * @return  true if success, false if failed
     */
    bool callback_send( callback_t *callback, EventBits_t event, void *arg );
    /**
     * @brief   call all callback function thats match with the event filter mask without logging
     * 
     * @param   callback        pointer to a callback_t structure
     * @param   event           event filter mask
     * @param   arg             argument for the called callback function
     * 
     * @return  true if success, false if failed
     */
    bool callback_send_no_log( callback_t *callback, EventBits_t event, void *arg );
    /**
     * @brief enable/disable SPIFFS event logging
     * 
     * @param enable    true if logging enabled, false if logging disabled
     */
    void display_event_logging_enable( bool enable );
    void callback_print( void );

#endif // _CALLBACK_H