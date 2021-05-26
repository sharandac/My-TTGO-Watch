#include "config.h"
#include <TTGO.h>
#include "utils/sqlite3/sqlite3.h"

#include "calendar_db.h"

sqlite3 *calendar_db = NULL;
static bool highlight_days[ 31 ];

static int calendar_db_exec_callback( void *data, int argc, char **argv, char **azColName );
static int calendar_db_highlight_day_callback( void *data, int argc, char **argv, char **azColName );
int calendar_db_exec( sqlite3 *db, const char *sql );

void calendar_db_setup( void ) {
    int retval = 0;
    bool create_table = false;
    /**
     * check if database exist and set create_table flag if not exist
     */
    #ifdef CALENDAR_DB_FORCE_CREATE_DB
        remove( CALENDAR_DB_FILE );
    #endif // CALENDAR_DB_FORCE_CREATE_DB
    FILE *dbfile;
    dbfile = fopen( CALENDAR_DB_FILE, "r" );
    if ( dbfile ) {
        CALENDAR_DB_DEBUG_LOG("database exist");
    }
    else {
        CALENDAR_DB_ERROR_LOG("databsae not exist, create database and tables");
        create_table = true;
        fclose( dbfile );
    }
    /**
     * init sqlite3
     */
    sqlite3_initialize();
    /**
     * open database
     */
    retval = sqlite3_open( CALENDAR_DB_FILE, &calendar_db );
    if ( retval ) {
        CALENDAR_DB_ERROR_LOG("Can't open database: %s", sqlite3_errmsg( calendar_db ) );
    }
    else {
        CALENDAR_DB_DEBUG_LOG("Opened database successfully");
        /**
         * create tables if create_table flag is set
         */
        if ( create_table ) {
            CALENDAR_DB_DEBUG_LOG("create tables");
            calendar_db_exec( calendar_db, "CREATE TABLE calendar ( year INTEGER, month INTEGER, day INTEGER, hour INTEGER, min INTEGER, done BOOL, content );");
            #ifdef CALENDAR_DB_CREATE_TEST_DATA
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 5, 17, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 5, 25, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 5, 13, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 5, 30, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 5, 12, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 5, 28, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 5, 17, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 6, 25, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 7, 13, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 8, 30, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 8, 12, 10, 0, false, 'Hello, World from calendar');");
                calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 9, 28, 10, 0, false, 'Hello, World from calendar');");
            #endif // CALENDAR_DB_CREATE_TEST_DATA
        }
        calendar_db_exec( calendar_db, "SELECT rowid, year, month, day, hour min, done, content FROM calendar;");
    }
    sqlite3_close( calendar_db );

    calendar_db = NULL;
}

bool calendar_db_open( void ) {
    bool error = false;
    /**
     * check if calendar_db already open
     */
    if ( calendar_db ) {
        return( error );
    }
    /**
     * open database
     */
    error = sqlite3_open( CALENDAR_DB_FILE, &calendar_db );
    if ( error ) {
        CALENDAR_DB_ERROR_LOG("can't open calendar database: %s", sqlite3_errmsg( calendar_db ) );
        calendar_db = NULL;
    }
    else {
        CALENDAR_DB_DEBUG_LOG("calendar database open");
    }
    return( error );
}

void calendar_db_close( void ) {
    if ( calendar_db ) {
        sqlite3_close( calendar_db );
        CALENDAR_DB_DEBUG_LOG("calendar database closed");
    }
    calendar_db = NULL;
}

const char* data = "Callback function called";
static int calendar_db_exec_callback( void *data, int argc, char **argv, char **azColName ) {
   String Result = "";
   for ( int i = 0; i < argc ; i++ ) {
       Result += i != 0 ? "," : "";
       Result += azColName[i];
       Result += "=";
       Result += argv[i] ? argv[i] : "NULL";
   }
   CALENDAR_DB_DEBUG_LOG("Result = %s", Result.c_str() );
   return 0;
}

char *zErrMsg = 0;
int calendar_db_exec( sqlite3 *db, const char *sql ) {
   long start = millis();

   CALENDAR_DB_DEBUG_LOG("sql: %s",sql );   
   int retval = sqlite3_exec( db, sql, calendar_db_exec_callback, (void*)data, &zErrMsg);
   if ( retval != SQLITE_OK ) {
       CALENDAR_DB_ERROR_LOG("SQL error: %s", zErrMsg );
       sqlite3_free(zErrMsg);
   } 
   CALENDAR_DB_DEBUG_LOG("Time taken: %dms", millis() - start );
   return retval;
}

static int calendar_db_highlight_day_callback( void *data, int argc, char **argv, char **azColName ) {
    String Result = "";
    /**
     * count all key/values pairs
     */
    for ( int i = 0; i < argc ; i++ ) {
       /**
         * build an result string for one line presentation
         */
        Result += i != 0 ? "," : "";
        Result += azColName[i];
        Result += "=";
        Result += argv[i] ? argv[i] : "NULL";
        /**
          * mark day in highlight_days as a day with an date
          */
         if ( !strcmp( azColName[ i ], "day") ) {
             highlight_days[ atoi( argv[ i ] ) ] = true;
            CALENDAR_DB_DEBUG_LOG("mark day %d as red", atoi( argv[ i ] ) );
        }
    }
    CALENDAR_DB_DEBUG_LOG("Result = %s", Result.c_str() );
    return 0;
}

int calendar_db_highlight_day( lv_calendar_date_t *highlighted_days, int year, int month ) {
    char *zErrMsg = 0;
    long start = millis();
    char sql[ 256 ];
    int hitcounter = 0;
    /**
     * check if database already open
     */
    if ( !calendar_db ) {
        return( 0 );
    }
    /**
     * clear highlight_days table
     */
    for ( int i = 0 ; i < 31 ; i++ ) {
        highlight_days[ i ] = false;
    }
    /**
     * build sql query string
     */
    snprintf( sql, sizeof( sql ), "SELECT rowid, year, month, day, hour min, done, content FROM calendar WHERE year == %d AND month == %d;", year, month );
    /**
     * exec sql query
     */
    CALENDAR_DB_DEBUG_LOG("sql: %s",sql );
    int retval = sqlite3_exec( calendar_db, sql , calendar_db_highlight_day_callback, (void*)data, &zErrMsg);
    if ( retval != SQLITE_OK ) {
        CALENDAR_DB_ERROR_LOG("SQL error: %s", zErrMsg );
        sqlite3_free(zErrMsg);
    } 
    else {
        /**
         * count day with day and marked days with dates
         */
        for ( int i = 0 ; i < 31 ; i++ ) {
            if ( highlight_days[ i ] ) {
                highlighted_days[ hitcounter ].day = i;
                highlighted_days[ hitcounter ].month = month;
                highlighted_days[ hitcounter ].year = year;
                hitcounter++;
            }
        }
    }
    CALENDAR_DB_DEBUG_LOG("Time taken: %dms", millis() - start );

    return( hitcounter );
}