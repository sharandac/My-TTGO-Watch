#include "IRConfig.h"
#include "utils/alloc.h"

#ifdef NATIVE_64BIT
#else
    #if defined( M5PAPER ) || defined( M5CORE2 ) || defined( LILYGO_WATCH_2021 ) 

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <IRremoteESP8266.h>
        IRConfig::IRConfig() : BaseJsonConfig("ir-remote.json") {
            count = 0;
            // This file is too big for that!
            prettyJson = false;
        }

        InfraButton* IRConfig::add(size_t page, const char* name) {
            void* pointer = MALLOC(sizeof(InfraButton));
            InfraButton* btn = new (pointer) InfraButton();
            btn->page = page;
            btn->name = name;
            buttons[count++] = btn;
            return btn;
        }

        void IRConfig::del(size_t page, const char* name) {
        bool found = false;
        for (int i = 0; i < count; i++) {
            if (buttons[i]->page == page && buttons[i]->name == name) {
            if (buttons[i]->uiButton.isCreated()) {
                buttons[i]->uiButton.free();
            }
            delete buttons[i];
            found = true;
            }
            if (found)
            buttons[i] = buttons[i+1];
        }
        if (found)
            count--;
        }

        InfraButton* IRConfig::get(size_t page, const char* name) {
        for (int i = 0; i < count; i++) {
            if (buttons[i]->page == page && buttons[i]->name == name)
            return buttons[i];
        }
        return nullptr;
        }

        void IRConfig::sendListNames(BluetoothJsonResponse& response) {
        auto nestedArray = response.createNestedArray("v");
        for (int i = 0; i < count; i++) {
            nestedArray.add(buttons[i]->name);
        }
        response.send();
        }

        void IRConfig::sendButtonEdit(BluetoothJsonResponse& response, size_t page, const char* name) {
        auto btn = get(page, name);
        if (btn != nullptr) {
            response["v"] = btn->name;
            response["m"] = (int)btn->mode;
            if (btn->mode == decode_type_t::RAW) {
            auto nestedArray = response.createNestedArray("raw");
            for (int i = 0; i < btn->rawLength; i++)
                nestedArray.add(btn->raw[i]);
            } else {
            response["hex"] = String(btn->code, 16);
            }
        }
        response.send();
        }

        bool IRConfig::onSave(JsonDocument& document) {
        auto pagesArray = document.createNestedArray("pages");
        auto main = pagesArray.createNestedObject();
        for (int i = 0; i < count; i++) {
            JsonObject btnRecord = main.createNestedObject(buttons[i]->name);
            btnRecord["m"] = buttons[i]->mode;
            String hex((uint32_t)buttons[i]->code, 16);
            btnRecord["hex"] = hex;
            if (buttons[i]->bits > 0 && buttons[i]->mode == decode_type_t::SONY) {
            btnRecord["bits"] = buttons[i]->bits;
            }
            if (buttons[i]->mode == decode_type_t::RAW) {
            auto rawArray = btnRecord.createNestedArray("raw");
            for (int j = 0; j < buttons[i]->rawLength; j++)
                rawArray.add(buttons[i]->raw[j]);
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
        
        pageCount = pages.size();
        for (size_t i = 0; i < pageCount; i++)
        {
            JsonObject main = pages[i].as<JsonObject>();
            if (main.isNull()) continue;

            for (JsonPair record : main) {
            if (record.value().isNull()) continue;
            
            // Create and load button
            log_d("Loading ir button: %s", record.key().c_str());
            auto btn = add(i, record.key().c_str());
            JsonObject configuration = record.value().as<JsonObject>();
            btn->loadFrom(configuration);
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

