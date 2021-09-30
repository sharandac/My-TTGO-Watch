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
#include "IRConfig.h"
#include "IRController.h"
#include "quickglui/quickglui.h"
#include "gui/mainbar/mainbar.h"
#include "gui/widget_styles.h"
#include "hardware/blectl.h"


#ifdef NATIVE_64BIT
    void IRController_setup( void ) {
        return;
    }
#else
    #if defined( M5PAPER ) ||  defined( M5CORE2 ) || defined( LILYGO_WATCH_2021 )    
        void IRController_setup( void ) {
            return;
        }
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <TTGO.h>
        #include <IRremoteESP8266.h>
        #include <IRsend.h>
        // App icon must have an size of 64x64 pixel with an alpha channel
        // Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
        LV_IMG_DECLARE(IRController_64px);

        static lv_point_t* valid_pos;
        static bool IRController_bluetooth_event_cb(EventBits_t event, void *arg);

        IRConfig irConfig;
        Application irController;
        IRsend irsend(TWATCH_2020_IR_PIN);
        lv_style_t irDeskStyle;

        /*
        * setup routine for IR Controller app
        */
        void IRController_setup( void ) {
            irController.init("IR Remote", &IRController_64px);
            // Load config and build user interface
            IRController_build_UI(IRControlSettingsAction::Load);

            blectl_register_cb(BLECTL_MSG_JSON, IRController_bluetooth_event_cb, "ir-remote setup");
        }

        void IRController_build_UI(IRControlSettingsAction settingsAction)
        {
            if (settingsAction == IRControlSettingsAction::Load)
                irConfig.load();

            AppPage& main = irController.mainPage();
            lv_obj_t *desks = lv_tileview_create(main.handle(), NULL);
            lv_obj_set_size(desks, LV_HOR_RES, LV_VER_RES);
            lv_page_set_scrollbar_mode(desks, LV_SCROLLBAR_MODE_OFF);
            lv_obj_add_style(desks, LV_OBJ_PART_MAIN, ws_get_mainbar_style());

            valid_pos = ( lv_point_t * )MALLOC( sizeof( lv_point_t ) * irConfig.pageCount );
            for (lv_coord_t i = 0; i < irConfig.pageCount; i++)
            {
                valid_pos[i].x = i;
                valid_pos[i].y = 0;
            }
            
            for (size_t i = 0; i < irConfig.pageCount; i++)
            {
                lv_obj_t * tile = lv_obj_create(desks, NULL);
                lv_obj_set_size(tile, LV_HOR_RES, LV_VER_RES);
                lv_obj_set_pos(tile, valid_pos[i].x * LV_HOR_RES, valid_pos[i].y * LV_VER_RES);
                lv_obj_add_style(tile, LV_OBJ_PART_MAIN, ws_get_mainbar_style());

                lv_obj_t * desk = lv_cont_create(tile, NULL);
                lv_obj_set_size(desk, LV_HOR_RES, LV_VER_RES - 42);
                lv_cont_set_layout(desk, LV_LAYOUT_PRETTY_MID);

                lv_style_copy(&irDeskStyle, ws_get_mainbar_style());
                lv_style_set_pad_inner(&irDeskStyle, LV_STATE_DEFAULT, irConfig.defSpacing);
                lv_style_set_pad_top(&irDeskStyle, LV_STATE_DEFAULT, 7);
                lv_style_set_pad_bottom(&irDeskStyle, LV_STATE_DEFAULT, 7);
                lv_style_set_pad_left(&irDeskStyle, LV_STATE_DEFAULT, 16);
                lv_style_set_pad_right(&irDeskStyle, LV_STATE_DEFAULT, 16);
                lv_obj_add_style(desk, LV_OBJ_PART_MAIN, &irDeskStyle);

                for (int j = 0; j < irConfig.totalCount(); j++)
                {
                    auto btnConfig = irConfig.get(j);
                    if (btnConfig->page != i) continue;

                    if (btnConfig->uiButton.isCreated()) {
                        btnConfig->uiButton.alignInParentTopLeft(0, 0); // Call auto alignment
                        continue;
                    }
                    
                    lv_obj_t * button = lv_btn_create(desk, NULL);
                    lv_label_create(button, NULL);

                    Button btn(button);
                    btn.size(irConfig.defBtnWidth, irConfig.defBtnHeight);
                    btn.text(btnConfig->name.c_str());
                    btn.clicked([btnConfig](Widget& btn) {
                        execute_ir_cmd(btnConfig);
                    });
                    btnConfig->uiButton = btn;
                }

                lv_tileview_add_element(desks, tile);
            }
            
            lv_tileview_set_valid_positions(desks, valid_pos, irConfig.pageCount);
            lv_tileview_set_edge_flash(desks, true);
            main.moveExitButtonToForeground();

            // Refresh screen
            lv_obj_invalidate(lv_scr_act());

            if (settingsAction == IRControlSettingsAction::Save)
                irConfig.save();
        }

        void execute_ir_cmd(InfraButton* config) {
            pinMode(TWATCH_2020_IR_PIN, OUTPUT);
            digitalWrite(TWATCH_2020_IR_PIN, LOW); // No Current Limiting so keep it off (!!!)

            switch (config->mode)
            {
            case RC5:
                irsend.sendRC5(config->code);
                break;
            case RC6:
                irsend.sendRC6(config->code);
                break;
            case NEC:
                irsend.sendNEC(config->code);
                break;
            case SONY:
                if (config->bits > 0)
                    irsend.sendSony(config->code, config->bits);
                else
                    irsend.sendSony(config->code);
                break;
            case PANASONIC:
                irsend.sendPanasonic64(config->code);
                break;
            case JVC:
                irsend.sendJVC(config->code);
                break;
            case SAMSUNG:
                irsend.sendSAMSUNG(config->code);
                break;
            case SAMSUNG36:
                irsend.sendSamsung36(config->code);
                break;
            case LG:
                irsend.sendLG(config->code);
                break;
            case SHARP:
                irsend.sendSharpRaw(config->code);
                break;
            case RAW:
                if (config->raw != nullptr && config->rawLength > 0)
                    irsend.sendRaw(config->raw, config->rawLength, 38);
                break;
            default:
                log_e("IR Protocol %d not supported, please add it first!", (int)config->mode);
                break;
            }

            delay(50);
            digitalWrite(TWATCH_2020_IR_PIN, LOW); // No Current Limiting so keep it off (!!!)
            log_i("IR button clicked: %s", config->name);
        }

        bool IRController_bluetooth_event_cb(EventBits_t event, void *arg) {
            if (event != BLECTL_MSG_JSON) return false; // Not supported

            InfraButton* btn = nullptr;
            BluetoothJsonRequest &request = *(BluetoothJsonRequest *)arg;

            if (request.isRequest() && request.isForApplication("ir"))
            {
                BluetoothJsonResponse response(request);
                String cmd = request.command(); // Requested command
                log_i("RECEIVED cmd: %s", cmd.c_str());
                if (cmd == "list") {
                    irConfig.sendListNames(response);
                } else if (cmd == "edit" || cmd == "save") {
                    String name = request["v"];
                    if (cmd == "save") {
                        // Update button data:
                        btn = irConfig.get(0, name.c_str());
                        if (btn != nullptr) {
                            JsonObject obj = request.as<JsonObject>();
                            btn->loadFrom(obj);
                            irConfig.save();
                        }
                    }
                    irConfig.sendButtonEdit(response, 0, name.c_str());
                } else if (cmd == "add") {
                    String name = request["v"];
                    btn = irConfig.add(0, name.c_str());
                    irConfig.sendListNames(response);
                    IRController_build_UI(IRControlSettingsAction::Save);
                } else if (cmd == "del") {
                    String name = request["v"];
                    irConfig.del(0, name.c_str());
                    irConfig.sendListNames(response);
                    IRController_build_UI(IRControlSettingsAction::Save);
                }
            }

            return true;
        }
    #else // NEW_HARDWARE_TAG
        void IRController_setup( void ) {
            return;
        }
    #endif
#endif

