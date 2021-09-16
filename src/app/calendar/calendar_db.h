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
#ifndef _CALENDAR_DB_H
    #define _CALENDAR_DB_H

    // #define CALENDAR_DB_FORCE_CREATE_DB
    #define CALENDAR_DB_CREATE_TEST_DATA

    #define CALENDAR_DB_INFO_LOG    log_i
    #define CALENDAR_DB_DEBUG_LOG   log_d
    #define CALENDAR_DB_ERROR_LOG   log_e

//    #define CALENDAR_DB_FILE        "/home/sharan/.hedge/spiffs/calendar.db"       /** @brief calendar database file */
    #define CALENDAR_DB_FILE        "/spiffs/calendar.db"       /** @brief calendar database file */
    /**
     * @brief sql exec callback function definition, this function is called on every result line
     * 
     * @param   data        user define data
     * @param   argc        number of arguments
     * @param   argv        arguments pointer table
     * @param   azColName   colum name pointer table
     */
    typedef int ( * SQL_CALLBACK_FUNC ) ( void *data, int argc, char **argv, char **azColName );
    /**
     * @brief setup sqlite3 interface and check if a database exist
     */
    void calendar_db_setup( void );
    /**
     * @brief open calendar db and holds it open in background
     * 
     * @return  true if no error, false if failed
     */
    bool calendar_db_open( void );
    /**
     * @brief close calendar db
     */
    void calendar_db_close( void );
    /**
     * @brief query an sql request
     * 
     * @param   callback    pointer to a callback funtion for the results
     * @param   sql         pointer to a sql query string
     * 
     * @return  true if was success or false if was failed
     */
    bool calendar_db_exec( SQL_CALLBACK_FUNC callback, const char *sql );
#endif // _CALENDAR_DB_H