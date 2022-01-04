/****************************************************************************
 *   January 04 19:00:00 2022
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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
#include "config.h"

#include "printer3d_app.h"
#include "printer3d_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/wifictl.h"
#include "utils/json_psram_allocator.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>

    using namespace std;
    #define String string
#else
    #include <Arduino.h>
    #include "HTTPClient.h"
#endif

bool printer3d_state = false;
volatile bool printer3d_open_state = false;
static uint64_t nextmillis = 0;

lv_obj_t *printer3d_app_main_tile = NULL;

lv_task_t * _printer3d_app_task;

LV_IMG_DECLARE(refresh_32px);

static void printer3d_setup_activate_callback ( void );
static void printer3d_setup_hibernate_callback ( void );
static void exit_printer3d_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_printer3d_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static bool printer3d_main_wifictl_event_cb( EventBits_t event, void *arg );

bool printer3d_refresh();
void printer3d_send(WiFiClient client, char* buffer, const char* command);
void printer3d_app_task( lv_task_t * task );

void printer3d_app_main_setup( uint32_t tile_num ) {

    mainbar_add_tile_activate_cb( tile_num, printer3d_setup_activate_callback );
    mainbar_add_tile_hibernate_cb( tile_num, printer3d_setup_hibernate_callback );
    printer3d_app_main_tile = mainbar_get_tile_obj( tile_num );

    lv_obj_t * exit_btn = wf_add_exit_button( printer3d_app_main_tile, exit_printer3d_app_main_event_cb );
    lv_obj_align(exit_btn, printer3d_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_ICON_PADDING, -THEME_ICON_PADDING );

    lv_obj_t * setup_btn = wf_add_setup_button( printer3d_app_main_tile, enter_printer3d_app_setup_event_cb );
    lv_obj_align(setup_btn, printer3d_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_ICON_PADDING, -THEME_ICON_PADDING );

    // callbacks
    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT_IP | WIFICTL_DISCONNECT, printer3d_main_wifictl_event_cb, "printer3d main" );

    // create an task that runs every fifteen seconds
    _printer3d_app_task = lv_task_create( printer3d_app_task, 15000, LV_TASK_PRIO_MID, NULL );
}

static void printer3d_setup_activate_callback ( void ) {
    printer3d_open_state = true;
    nextmillis = 0;
}

static void printer3d_setup_hibernate_callback ( void ) {
    printer3d_open_state = false;
    nextmillis = 0;
}

static bool printer3d_main_wifictl_event_cb( EventBits_t event, void *arg ) {    
    switch( event ) {
        case WIFICTL_CONNECT_IP:    printer3d_state = true;
                                    printer3d_app_set_indicator( ICON_INDICATOR_UPDATE );
                                    break;
        case WIFICTL_DISCONNECT:    printer3d_state = false;
                                    printer3d_app_set_indicator( ICON_INDICATOR_FAIL );
                                    break;
        case WIFICTL_OFF:           printer3d_state = false;
                                    printer3d_app_hide_indicator();
                                    break;
    }
    return( true );
}

static void enter_printer3d_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( printer3d_app_get_app_setup_tile_num(), LV_ANIM_ON );
                                        statusbar_hide( true );
                                        nextmillis = 0;
                                        break;
    }
}

static void exit_printer3d_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       printer3d_open_state = false;
                                        mainbar_jump_back();
                                        break;
    }
}

void printer3d_app_task( lv_task_t * task ) {
    if (!printer3d_state) return;

    if ( nextmillis < millis() ) {
        if (printer3d_open_state) {
            nextmillis = millis() + 15000L;
        } else {
            nextmillis = millis() + 60000L;
        }

        printer3d_app_set_indicator( ICON_INDICATOR_UPDATE );
        if (printer3d_refresh()) {
            printer3d_app_set_indicator( ICON_INDICATOR_OK );
        } else {
            printer3d_app_set_indicator( ICON_INDICATOR_FAIL );
        }
    }
}

bool printer3d_refresh() {
    if (!printer3d_state) return false;

    printer3d_config_t *printer3d_config = printer3d_get_config();
    if (!strlen(printer3d_config->host)) return false;

    // connecting to 3d printer
    WiFiClient client;
    client.connect(printer3d_config->host, printer3d_config->port, 3000);

    for (uint8_t i = 0; i < 30; i++) {
        if (client.connected()) break;
        delay(100);
    }

    if (!client.connected()){
        log_w("printer3d: could not connect to %s:%d", printer3d_config->host, printer3d_config->port);
        return false;
    }

    // sending G-Codes to 3d printer
    char* generalInfo = (char*)MALLOC( 1024 );
    char* stateInfo = (char*)MALLOC( 1024 );
    char* tempInfo = (char*)MALLOC( 512 );
    char* printInfo = (char*)MALLOC( 512 );
    printer3d_send(client, generalInfo, "~M115");
    printer3d_send(client, stateInfo, "~M119");
    printer3d_send(client, tempInfo, "~M105");
    printer3d_send(client, printInfo, "~M27");

    // close connection
    client.stop();

    // parse received information from the 3d printer
    if (generalInfo != NULL && strlen(generalInfo) > 0) {
        char machineType[32], machineVersion[16];

        char* generalInfoType = strstr(generalInfo, "Machine Type:");
        if ( generalInfoType != NULL && strlen(generalInfoType) > 0 && sscanf( generalInfoType, "Machine Type: %s", machineType ) > 0 ) {
            //TODO: Use the values to display them in the GUI
            log_i("3dprinter Type: %s", machineType);
        }

        char* generalInfoVersion = strstr(generalInfo, "Firmware:");
        if ( generalInfoVersion != NULL && strlen(generalInfoVersion) > 0 && sscanf( generalInfoVersion, "Firmware: %s", machineVersion ) > 0 ) {
            //TODO: Use the values to display them in the GUI
            log_i("3dprinter Version: %s", machineVersion);
        }
    }
    free( generalInfo );
    
    if (stateInfo != NULL && strlen(stateInfo) > 0) {
        char stateMachine[16], stateMove[16];

        char* stateInfoMachine = strstr(stateInfo, "MachineStatus:");
        if ( stateInfoMachine != NULL && strlen(stateInfoMachine) > 0 && sscanf( stateInfoMachine, "MachineStatus: %s", stateMachine ) > 0 ) {
            //TODO: Use the values to display them in the GUI
            log_i("3dprinter State: %s", stateMachine);
        }

        char* stateInfoMove = strstr(stateInfo, "MoveMode:");
        if ( stateInfoMove != NULL && strlen(stateInfoMove) > 0 && sscanf( stateInfoMove, "MoveMode: %s", stateMove ) > 0 ) {
            //TODO: Use the values to display them in the GUI
            log_i("3dprinter Move: %s", stateMove);
        }
    }
    free( stateInfo );

    if (tempInfo != NULL && strlen(tempInfo) > 0) {
        int extruderTemp, printbedTemp;
        
        char* tempInfoLine = strstr(tempInfo, "T0:");
        if ( tempInfoLine != NULL && strlen(tempInfoLine) > 0 && sscanf( tempInfoLine, "T0:%d /0 B:%d/0", &extruderTemp, &printbedTemp ) > 0 ) {
            //TODO: Use the values to display them in the GUI
            log_i("3dprinter Extruder: %d°C", extruderTemp);
            log_i("3dprinter PrintBed: %d°C", printbedTemp);
        }
    }
    free( tempInfo );
    
    if (printInfo != NULL && strlen(printInfo) > 0) {
        int printProgress, printMax;
        
        char* printInfoLine = strstr(printInfo, "byte ");
        if ( printInfoLine != NULL && strlen(printInfoLine) > 0 && sscanf( printInfoLine, "byte %d/%d", &printProgress, &printMax ) > 0 ) {
            //TODO: Use the values to display them in the GUI
            log_i("3dprinter Progress: %d/%d", printProgress, printMax);
        }
    }
    free( printInfo );

    return true;
}

void printer3d_send(WiFiClient client, char* buffer, const char* command) {
    if (!printer3d_state) return;

    client.write(command);
    log_d("3dprinter sent command: %s", command);
    
    for (uint8_t i = 0; i < 25; i++) {
        if (client.available()) break;
        delay(10);
    }
    
    while (client.available()) {
        client.readBytes(buffer, 512);
    }
    
    log_d("3dprinter received: %s", buffer);
}
