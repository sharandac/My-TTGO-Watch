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
#include "config.h"
#include <TTGO.h>
#include <BLEServer.h>
#include <BLEAdvertising.h>
#include "BLEHIDDevice.h"

#include "tiltmouse_app.h"
#include "tiltmouse_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/blectl.h"
#include "hardware/pmu.h"

lv_obj_t *tiltmouse_app_main_tile = NULL;
lv_style_t tiltmouse_app_main_style;

bool tiltmouse_active = false;
BLEHIDDevice *pHID;
BLECharacteristic *pHIDMouse;
lv_task_t * _tiltmouse_app_task;

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
LV_FONT_DECLARE(Ubuntu_72px);

static const uint8_t _hidReportDescriptor[] = {
  USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x02, // USAGE (Mouse)
  COLLECTION(1),       0x01, // COLLECTION (Application)
  USAGE(1),            0x01, //   USAGE (Pointer)
  COLLECTION(1),       0x00, //   COLLECTION (Physical)
  // ------------------------------------------------- Buttons (Left, Right, Middle, Back, Forward)
  USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
  USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
  USAGE_MAXIMUM(1),    0x05, //     USAGE_MAXIMUM (Button 5)
  LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x05, //     REPORT_COUNT (5)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;5 button bits
  // ------------------------------------------------- Padding
  REPORT_SIZE(1),      0x03, //     REPORT_SIZE (3)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;3 bit padding
  // ------------------------------------------------- X/Y position, Wheel
  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x30, //     USAGE (X)
  USAGE(1),            0x31, //     USAGE (Y)
  USAGE(1),            0x38, //     USAGE (Wheel)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x03, //     REPORT_COUNT (3)
  HIDINPUT(1),         0x06, //     INPUT (Data, Variable, Relative) ;3 bytes (X,Y,Wheel)
  // ------------------------------------------------- Horizontal wheel
  USAGE_PAGE(1),       0x0c, //     USAGE PAGE (Consumer Devices)
  USAGE(2),      0x38, 0x02, //     USAGE (AC Pan)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  HIDINPUT(1),         0x06, //     INPUT (Data, Var, Rel)
  END_COLLECTION(0),         //   END_COLLECTION
  END_COLLECTION(0)          // END_COLLECTION
};

void tiltmouse_app_task( lv_task_t * task );
static void tiltmouse_left_event_cb( lv_obj_t * obj, lv_event_t event );
static void tiltmouse_right_event_cb( lv_obj_t * obj, lv_event_t event );
void tiltmouse_move(uint8_t b, signed char x, signed char y, signed char wheel, signed char hWheel);

void tiltmouse_app_main_setup( uint32_t tile_num ) {
    tiltmouse_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &tiltmouse_app_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = wf_add_exit_button( tiltmouse_app_main_tile, exit_tiltmouse_app_event_cb, &tiltmouse_app_main_style );
    lv_obj_align(exit_btn, tiltmouse_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    // left mouse button
	lv_obj_t *tiltmouse_left_btn = NULL;
    tiltmouse_left_btn = wf_add_button( tiltmouse_app_main_tile, "Left", 75, 150, tiltmouse_left_event_cb );
    lv_obj_align( tiltmouse_left_btn, NULL, LV_ALIGN_CENTER, -40, 0 );
    
    // right mouse button
	lv_obj_t *tiltmouse_right_btn = NULL;
    tiltmouse_right_btn = wf_add_button( tiltmouse_app_main_tile, "Right", 75, 150, tiltmouse_right_event_cb );
    lv_obj_align( tiltmouse_right_btn, NULL, LV_ALIGN_CENTER, 40, 0 );

    // create an task that runs every 100ms
    _tiltmouse_app_task = lv_task_create( tiltmouse_app_task, 100, LV_TASK_PRIO_MID, NULL );

    // prepare hid mouse
    //BLEServer *pServer = blectl_get_ble_server();

    //TODO: Fix panic on the line below!
    //pHID = new BLEHIDDevice(pServer);
    //pHIDMouse = pHID->inputReport(0); // <-- input REPORTID from report map

    //pHID->pnp(0x02, 0xe502, 0xa111, 0x0210);
    //pHID->hidInfo(0x00,0x02);
    //pHID->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
    //pHID->startServices();
}

void tiltmouse_app_task( lv_task_t * task ) {
    if ( !tiltmouse_active ) return;
    if ( blectl_get_event( BLECTL_OFF | BLECTL_DISCONNECT ) ) return;

    TTGOClass * ttgo = TTGOClass::getWatch();

    Accel acc;
    ttgo->bma->getAccel(acc);

    tiltmouse_move( 0, acc.x, acc.y, 0, 0 );
}

void tiltmouse_activate() {
    //BLEAdvertising *pAdvertising = blectl_get_ble_advertising();
    //pAdvertising->setAppearance( ESP_BLE_APPEARANCE_HID_MOUSE );
    //pAdvertising->addServiceUUID(pHID->hidService()->getUUID());
    //tiltmouse_active = true;
}

void tiltmouse_deactivate() {
    //BLEAdvertising *pAdvertising = blectl_get_ble_advertising();
    //pAdvertising->setAppearance( ESP_BLE_APPEARANCE_GENERIC_WATCH );
    //tiltmouse_active = true;
}

void tiltmouse_move(uint8_t b, signed char x, signed char y, signed char wheel, signed char hWheel)
{
    uint8_t m[5];
    m[0] = b;
    m[1] = x;
    m[2] = y;
    m[3] = wheel;
    m[4] = hWheel;
    pHIDMouse->setValue(m, 5);
    pHIDMouse->notify();
}

static void tiltmouse_left_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_CLICKED ):       tiltmouse_move( MOUSE_LEFT, 0, 0, 0, 0 );
                                        break;
    }
}

static void tiltmouse_right_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_CLICKED ):       tiltmouse_move( MOUSE_RIGHT, 0, 0, 0, 0 );
                                        break;
    }
}