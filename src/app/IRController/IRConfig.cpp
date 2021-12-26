#include "IRConfig.h"
#include "utils/alloc.h"

#ifdef NATIVE_64BIT
#else
    #if defined( M5PAPER ) || defined( M5CORE2 ) || defined( LILYGO_WATCH_2021 ) 

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <IRremoteESP8266.h>
        IRConfig::IRConfig() : BaseJsonConfig("ir-remote.json") {
            buttonCount = 0;
            prettyJson = false; // This file is too big for that!
        }

        InfraButton* IRConfig::add(size_t page, const char* name) {
            void* pointer = MALLOC(sizeof(InfraButton));
            InfraButton* btn = new (pointer) InfraButton();
            btn->page = page;
            strncpy( btn->name, name, sizeof( btn->name ) );
            buttons[buttonCount++] = btn;
            return btn;
        }

        void IRConfig::del(size_t page, const char* name) {
            bool found = false;
            for (int i = 0; i < buttonCount; i++) {
                if (buttons[i]->page == page && buttons[i]->name == name) {
                if (buttons[i]->uiButton.isCreated()) {
                    buttons[i]->uiButton.free();
                }
                delete &buttons[i];
                found = true;
                }
                if (found)
                buttons[i] = buttons[i+1];
            }
            if (found)
                buttonCount--;
        }

        InfraButton* IRConfig::get(size_t page, const char* name) {
            for (int i = 0; i < buttonCount; i++) {
                if (buttons[i]->page == page && buttons[i]->name == name)
                return buttons[i];
            }
            return nullptr;
        }

        void IRConfig::sendListNames(BluetoothJsonResponse& response) {
            auto nestedArray = response.createNestedArray("v");
            for (int i = 0; i < buttonCount; i++) {
                nestedArray.add(buttons[i]->name);
            }
            response.send();
        }

        void IRConfig::sendButtonEdit(BluetoothJsonResponse& response, size_t page, const char* name) {
            auto btn = get(page, name);

            auto command = btn->commands[0];
            response["v"] = btn->name;
            response["m"] = (int)command->mode;
            if (command->mode == decode_type_t::RAW) {
                auto nestedArray = response.createNestedArray("raw");
                for (int i = 0; i < command->rawLength; i++)
                    nestedArray.add(command->raw[i]);
            } else {
                response["hex"] = String(command->code, 16);
            }
            response.send();
        }

        bool IRConfig::onSave(JsonDocument& document) {
            auto pagesArray = document.createNestedArray("pages");
            auto main = pagesArray.createNestedObject();

            for (int i = 0; i < buttonCount; i++) {
                auto button = buttons[i];
                auto commands = button->commands;
                if (button->commandCount > 1) {
                    JsonArray btnRecords = main.createNestedArray(buttons[i]->name);
                    for (size_t j = 0; j < buttonCount; j++) {
                        JsonObject btnRecord = btnRecords.createNestedObject();
                        btnRecord["m"] = commands[j]->mode;
                        String hex((uint32_t)commands[j]->code, 16);
                        btnRecord["hex"] = hex;
                        if (commands[j]->bits > 0 && commands[j]->mode == decode_type_t::SONY) {
                            btnRecord["bits"] = commands[j]->bits;
                        }
                        if (commands[j]->mode == decode_type_t::RAW) {
                            auto rawArray = btnRecord.createNestedArray("raw");
                            for (int j = 0; j < commands[j]->rawLength; j++)
                                rawArray.add(commands[j]->raw[j]);
                        }
                    }
                    
                } else {
                    JsonObject btnRecord = main.createNestedObject(buttons[i]->name);
                    btnRecord["m"] = commands[0]->mode;
                    String hex((uint32_t)commands[0]->code, 16);
                    btnRecord["hex"] = hex;
                    if (commands[0]->bits > 0 && commands[0]->mode == decode_type_t::SONY) {
                        btnRecord["bits"] = commands[0]->bits;
                    }
                    if (commands[0]->mode == decode_type_t::RAW) {
                        auto rawArray = btnRecord.createNestedArray("raw");
                        for (int j = 0; j < commands[0]->rawLength; j++)
                            rawArray.add(commands[0]->raw[j]);
                    }
                }
            }

            document["defBtnHeight"] = defBtnHeight;
            document["defBtnWidth"] = defBtnWidth;
            document["defSpacing"] = defSpacing;

            return true;
        }


        bool IRConfig::onDefault( void ) {
            return true;
        }

        bool IRConfig::onLoad(JsonDocument& document) {
        JsonArray pages = document["pages"].as<JsonArray>();
        if (pages.isNull() || pages.size() < 1) return false;
        
        size_t jsonButtonCount = 0;
        pageCount = pages.size();
        for (size_t i = 0; i < pageCount; i++) {
            jsonButtonCount += pages[i].size();
        }

        // Increase buttons treshold further and leave space for adding buttons via bluetooth
        if (jsonButtonCount > IR_BUTTONS_START - IR_BUTTONS_TRESHOLD) {
            if (buttons != nullptr) free(buttons);
            buttons = (InfraButton**)MALLOC(sizeof( InfraButton* ) * (jsonButtonCount + IR_BUTTONS_TRESHOLD));
            if (buttons) {
                log_d("alloc more infrared buttons from %d to %d successful", IR_BUTTONS_START, (jsonButtonCount + IR_BUTTONS_TRESHOLD));
            }
            else {
                log_e("alloc more infrared buttons failed");
                while( true );
            }
        } else if (buttons == nullptr) {
            buttons = (InfraButton**)MALLOC(sizeof( InfraButton* ) * IR_BUTTONS_START);
        }

        for (size_t i = 0; i < pageCount; i++)
        {
            JsonObject main = pages[i].as<JsonObject>();
            if (main.isNull()) continue;

            for (JsonPair record : main) {
                if (record.value().isNull()) continue;
                
                // Create and load button
                log_d("Loading ir button: %s", record.key().c_str());
                auto btn = add(i, record.key().c_str());
                
                // Check if button has multiple command definitions (JsonArray)
                if (record.value().is<JsonArray>()) {
                    // Use multiple command definitions
                    JsonArray configuration = record.value().as<JsonArray>();
                    btn->loadFrom(configuration);
                } else {
                    // Use single command definition
                    JsonObject configuration = record.value().as<JsonObject>();
                    btn->loadFrom(configuration);
                }
            }
        }
        
        if (document.containsKey("defBtnHeight"))
            defBtnHeight = document["defBtnHeight"];
        if (document.containsKey("defBtnWidth"))
            defBtnWidth = document["defBtnWidth"];
        if (document.containsKey("defSpacing"))
            defSpacing = document["defSpacing"];
        if (defBtnHeight < 12 || defBtnHeight > 200)
            defBtnHeight = 12;
        if (defBtnWidth < 50 || defBtnWidth > LV_HOR_RES-10)
            defBtnWidth = 50;
        if (defSpacing < 0 || defSpacing > 100)
            defSpacing = 5;

        return true;
        }
    #else // NEW_HARDWARE_TAG
    #endif
#endif

