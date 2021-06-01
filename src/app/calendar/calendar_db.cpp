#include "config.h"
#include <TTGO.h>
#include "utils/sqlite3/sqlite3.h"

#include "calendar_db.h"
/**
 * internal variables
 */
sqlite3 *calendar_db = NULL;
static uint32_t calendar_db_version = 0;
/**
 * internal function declaration
 */
static int calendar_db_version_callback( void *data, int argc, char **argv, char **azColName );
static int calendar_db_exec_callback( void *data, int argc, char **argv, char **azColName );

void calendar_db_setup( void ) {
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
        fclose( dbfile );
        /**
         * get calendar_db_version
         */
        if ( calendar_db_open() ) {
            calendar_db_exec( calendar_db_version_callback, "SELECT version FROM calendar_db_version;");
        }
        calendar_db_close();
    }
    else {
        CALENDAR_DB_ERROR_LOG("databsae not exist, create database and tables");
        /**
         * open database
         */
        if ( calendar_db_open() ) {
            CALENDAR_DB_INFO_LOG("Opened database successfully");
            /**
             * create tables
             */
            CALENDAR_DB_INFO_LOG("create tables");
            calendar_db_exec( calendar_db_exec_callback, "CREATE TABLE calendar ( date INTEGER, year INTEGER, month INTEGER, day INTEGER, hour INTEGER, min INTEGER, content );");
            calendar_db_exec( calendar_db_exec_callback, "CREATE TABLE calendar_db_version ( version INTEGER );");
            calendar_db_exec( calendar_db_exec_callback, "INSERT INTO calendar_db_version VALUES ( 1 );");
            #ifdef CALENDAR_DB_CREATE_TEST_DATA
                CALENDAR_DB_INFO_LOG("create test dataset");
                /**
                 * Set today's date
                 */
                time_t now;
                struct tm time_tm;
                time( &now );
                localtime_r( &now, &time_tm );
                char date[30]="";
                snprintf( date, sizeof( date ),"%04d%02d%02d%02d%02d", ( time_tm.tm_year + 1900 ), ( time_tm.tm_mon + 1 ), time_tm.tm_mday, time_tm.tm_hour, time_tm.tm_min );
                String sql = (String)   "INSERT INTO calendar VALUES (" +
                                        date + "," +
                                        ( time_tm.tm_year + 1900 ) + "," + 
                                        ( time_tm.tm_mon + 1 ) + "," + 
                                        time_tm.tm_mday + "," + 
                                        time_tm.tm_hour + "," + 
                                        time_tm.tm_min + "," + 
                                        "'first date');";
                calendar_db_exec( calendar_db_exec_callback, sql.c_str() );
                calendar_db_exec( calendar_db_version_callback, "SELECT version FROM calendar_db_version;");
            #endif // CALENDAR_DB_CREATE_TEST_DATA
            /**
             * close and shutdown sqlite3
             */
            calendar_db_close();
        }
        else {
            CALENDAR_DB_ERROR_LOG("Can't open database: %s", sqlite3_errmsg( calendar_db ) );
        }
    }
}

bool calendar_db_open( void ) {
    bool retval = false;
    /**
     * check if calendar_db already open
     */
    if ( calendar_db ) {
        CALENDAR_DB_INFO_LOG("calendar_db already open");
        return( true );
    }
    /**
     * init sqlite3
     */
    sqlite3_initialize();
    /**
     * open database
     */
    int error = sqlite3_open( CALENDAR_DB_FILE, &calendar_db );
    if ( error ) {
        CALENDAR_DB_ERROR_LOG("can't open calendar database: %s", sqlite3_errmsg( calendar_db ) );
        calendar_db = NULL;
        /**
         * shutdown sqlite3
         */
        sqlite3_db_cacheflush( calendar_db );
        sqlite3_close( calendar_db );
        sqlite3_db_release_memory( calendar_db );
        sqlite3_shutdown();
    }
    else {
        CALENDAR_DB_DEBUG_LOG("calendar database open");
        retval = true;
    }
    return( retval );
}

void calendar_db_close( void ) {
    if ( calendar_db ) {
        /**
         * shutdown sqlite3
         */
        sqlite3_db_cacheflush( calendar_db );
        sqlite3_close( calendar_db );
        sqlite3_db_release_memory( calendar_db );
        sqlite3_shutdown();
        CALENDAR_DB_DEBUG_LOG("calendar database closed");
    }
    calendar_db = NULL;
}

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

static int calendar_db_version_callback( void *data, int argc, char **argv, char **azColName ) {
   String Result = "";
   for ( int i = 0; i < argc ; i++ ) {
       if ( !strcmp( azColName[i], "version" ) ) {
           calendar_db_version = atoi( argv[i] );
           CALENDAR_DB_INFO_LOG("calendar_db_version = %d", calendar_db_version );
       }
   }
   return 0;
}

bool calendar_db_exec( SQL_CALLBACK_FUNC callback, const char *sql ) {
    char *zErrMsg = NULL;
    bool retval = false;
    long query_start = millis();
    long query_time = 0;
    /**
     * check if database already open
     */
    if ( !calendar_db ) {
        return( 0 );
    }
    /**
     * query sql request
     */
    CALENDAR_DB_INFO_LOG("sql: %s",sql );   
    int sql_reval = sqlite3_exec( calendar_db, sql, callback, NULL, &zErrMsg);
    if ( sql_reval != SQLITE_OK ) {
        CALENDAR_DB_ERROR_LOG("SQL error: %s", zErrMsg );
        sqlite3_free(zErrMsg);
    } 
    else {
        retval = true;
    }
    query_time = millis() - query_start;
    CALENDAR_DB_INFO_LOG("query time = %dms", query_time );
    return retval;
}
