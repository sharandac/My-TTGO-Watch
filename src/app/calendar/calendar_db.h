#ifndef _CALENDAR_DB_H
    #define _CALENDAR_DB_H

    #define CALENDAR_DB_FILE        "/spiffs/calendar.db"       /** @brief calendar database file */

    /**
     * @brief setup sqlite3 interface and check if a database exist
     */
    void calendar_db_setup( void );

#endif // _CALENDAR_DB_H