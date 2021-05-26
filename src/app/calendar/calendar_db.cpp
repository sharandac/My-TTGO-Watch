#include "config.h"
#include <TTGO.h>
#include "utils/sqlite3/sqlite3.h"

#include "calendar_db.h"

sqlite3 *calendar_db = NULL;

static int calendar_db_exec_callback( void *data, int argc, char **argv, char **azColName );
int calendar_db_exec( sqlite3 *db, const char *sql );

void calendar_db_setup( void ) {
    int retval = 0;
    bool create_table = false;
    /**
     * check if database exist and set create_table flag if not exist
     */
    FILE *dbfile;
    dbfile = fopen( CALENDAR_DB_FILE, "r" );
    if ( dbfile ) {
        log_i("database exist");
    }
    else {
        log_e("databsae not exist, create database and tables");
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
        log_e("Can't open database: %s", sqlite3_errmsg( calendar_db ) );
    }
    else {
        log_i("Opened database successfully");
        /**
         * create tables if create_table flag is set
         */
        if ( create_table ) {
            log_i("create tables");
            calendar_db_exec( calendar_db, "CREATE TABLE calendar ( year INTEGER, month INTEGER, day INTEGER, hour INTEGER, min INTEGER, done BOOL, content );");
            calendar_db_exec( calendar_db, "INSERT INTO calendar VALUES ( 2021, 5, 17, 10, 0, false, 'Hello, World from calendar');");
        }
        calendar_db_exec( calendar_db, "SELECT rowid, year, month, day, hour min, done, content FROM calendar WHERE done != false LIMIT 1;");
    }
    sqlite3_close( calendar_db );

    calendar_db = NULL;
}

const char* data = "Callback function called";
static int calendar_db_exec_callback( void *data, int argc, char **argv, char **azColName ) {
   for ( int i = 0; i < argc ; i++ ) {
       log_i("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL" );
   }
   return 0;
}

char *zErrMsg = 0;
int calendar_db_exec( sqlite3 *db, const char *sql ) {
   long start = millis();

   log_i("sql: %s",sql );   
   int retval = sqlite3_exec( db, sql, calendar_db_exec_callback, (void*)data, &zErrMsg);
   if ( retval != SQLITE_OK ) {
       log_e("SQL error: %s", zErrMsg );
       sqlite3_free(zErrMsg);
   } 
   log_i("Time taken: %dms", millis() - start );
   return retval;
}