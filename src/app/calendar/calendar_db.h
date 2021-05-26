#ifndef _CALENDAR_DB_H
    #define _CALENDAR_DB_H

    // #define CALENDAR_DB_FORCE_CREATE_DB
    #define CALENDAR_DB_CREATE_TEST_DATA

    #define CALENDAR_DB_INFO_LOG    log_i
    #define CALENDAR_DB_DEBUG_LOG   log_i
    #define CALENDAR_DB_ERROR_LOG   log_e

    #define CALENDAR_DB_FILE        "/spiffs/calendar.db"       /** @brief calendar database file */

    /**
     * @brief setup sqlite3 interface and check if a database exist
     */
    void calendar_db_setup( void );
    /**
     * @brief open calendar db and holds it open in background
     * 
     * @return  true of a error, false if no error
     */
    bool calendar_db_open( void );
    /**
     * @brief close calendar db
     */
    void calendar_db_close( void );
    /**
     * @brief generate an highlighted_days table for calendar
     * 
     * @param   highlighted_days    pointer to an lv_calendar_date_t structure with 31 entrys
     * @param   year                current year
     * @param   month               current month
     * 
     * @return  number of highlighted entrys
     */
    int calendar_db_highlight_day( lv_calendar_date_t *highlighted_days, int year, int month );
#endif // _CALENDAR_DB_H