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

#include "IRController.h"
#include "quickglui/quickglui.h"

#include "gui/mainbar/mainbar.h"
#include "hardware/blectl.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include "IRConfig.h"

// App icon must have an size of 64x64 pixel with an alpha channel
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(IRController_64px);

IRConfig irConfig;
Application irController;
IRsend irsend(13);
Style irDeskStyle;

/*
 * setup routine for IR Controller app
 */
void IRController_setup( void ) {
    irController.init("IR Remote", &IRController_64px);
    // Load config and build user interface
    IRController_build_UI(IRControlSettingsAction::Load);

    blectl_register_cb(BLECTL_MSG, IRController_bluetooth_event_cb, "ir-remote setup");
}

void IRController_build_UI(IRControlSettingsAction settingsAction)
{
    if (settingsAction == IRControlSettingsAction::Load)
        irConfig.load();

    AppPage& main = irController.mainPage();
    // Create parent widget which will contains all IR control buttons
    // It also will auto-align child buttons on it:
    Container& desk = main.createChildContainer(LV_LAYOUT_PRETTY_MID);
    
    irDeskStyle = Style::Create(mainbar_get_style(), true);
    irDeskStyle.paddingInner(irConfig.defSpacing);
    irDeskStyle.padding(7, 16, 7, 16);
    desk.style(irDeskStyle);

    for (int i = 0; i < irConfig.totalCount(); i++)
    {
        auto btnConfig = irConfig.get(i);
        if (btnConfig->uiButton.isCreated()) {
            btnConfig->uiButton.alignInParentTopLeft(0, 0); // Call auto alignment
            continue;
        }
        
        // Add new button
        Button btn(&desk, btnConfig->name.c_str(), [btnConfig](Widget& btn) {
            execute_ir_cmd(btnConfig);
        });
        btn.size(irConfig.defBtnWidth, irConfig.defBtnHeight);
        btnConfig->uiButton = btn;
    }

    // Refresh screen
    lv_obj_invalidate(lv_scr_act());

    if (settingsAction == IRControlSettingsAction::Save)
        irConfig.save();
}

void execute_ir_cmd(InfraButton* config) {
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW); // No Current Limiting so keep it off (!!!)

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
    case SAMSUNG:
        irsend.sendSAMSUNG(config->code);
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
    digitalWrite(13, LOW); // No Current Limiting so keep it off (!!!)
    log_i("IR button clicked: %s", config->name);
}

bool IRController_bluetooth_event_cb(EventBits_t event, void *arg) {
    if (event != BLECTL_MSG) return false; // Not supported

    auto msg = (const char*)arg;
    InfraButton* btn = nullptr;
    BluetoothJsonRequest request(msg, strlen( msg ) * 4);

    if (request.isRequest() && request.isForApplication("ir"))
    {
        BluetoothJsonResponse response(request);
        String cmd = request.command(); // Requested command
        log_i("RECEIVED cmd: %s, msg: %s", cmd.c_str(), msg);
        if (cmd == "list") {
            irConfig.sendListNames(response);
        } else if (cmd == "edit" || cmd == "save") {
            String name = request["v"];
            if (cmd == "save") {
                // Update button data:
                btn = irConfig.get(name.c_str());
                if (btn != nullptr) {
                    JsonObject obj = request.as<JsonObject>();
                    btn->loadFrom(obj);
                    irConfig.save();
                }
            }
            irConfig.sendButtonEdit(response, name.c_str());
        } else if (cmd == "add") {
            String name = request["v"];
            btn = irConfig.add(name.c_str());
            irConfig.sendListNames(response);
            IRController_build_UI(IRControlSettingsAction::Save);
        } else if (cmd == "del") {
            String name = request["v"];
            irConfig.del(name.c_str());
            irConfig.sendListNames(response);
            IRController_build_UI(IRControlSettingsAction::Save);
        }
    }

    return true;
}