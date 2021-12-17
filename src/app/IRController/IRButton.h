#ifndef IR_BUTTON_H
#define IR_BUTTON_H

#include "config.h"
#include "utils/alloc.h"

#ifdef NATIVE_64BIT
#else
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <IRremoteESP8266.h>
        #define RAW_CODE_BUFFER_SIZE sizeof(uint16_t)*120

        struct InfraCommand
        {
            ~InfraCommand() {
                if (raw != nullptr) free(raw);
                raw = nullptr;
            }

            decode_type_t mode;
            uint32_t code = 0;
            int bits = 0;
            uint16_t *raw = nullptr;
            int rawLength = 0;

            void resize(int size)
            {
                if (raw == nullptr) 
                raw = (uint16_t*)MALLOC(RAW_CODE_BUFFER_SIZE);
                rawLength = size;
                memset(raw, 0, sizeof(uint16_t)*rawLength);
            }

            void loadFrom(JsonObject& source)
            {
                mode = (decode_type_t)(int)source["m"];
                
                if (source.containsKey("hex")) {
                    const char* hex = source["hex"];
                    if (hex != nullptr)
                        code = strtoul(hex, NULL, 16);
                }

                if (source.containsKey("bits")) {
                    bits = source["bits"];
                } else if (mode == decode_type_t::SONY && source.containsKey("hex")) {
                    const char* hex = source["hex"];
                    size_t hexLength = strlen(hex);
                    if (hexLength == 3) {
                        bits = 12;
                    } else if (hexLength == 4) {
                        bits = 15;
                    } // otherwise sony default length of 20 will be used.
                }

                if (mode == decode_type_t::RAW && source.containsKey("raw")) {
                    JsonArrayConst arr = source["raw"].as<JsonArray>();
                    if (!arr.isNull()) {
                        log_d("RAW size: %d", arr.size());
                        resize(arr.size());
                        for (int i=0; i < arr.size(); i++)
                        raw[i] = arr[i].as<uint16_t>();
                    }
                }
            }
        };

        struct InfraButton
        {
            ~InfraButton() {
                if (commands != nullptr) free(commands);
                commands = nullptr;
            }

            size_t page;
            char name[16];
            Button uiButton;
            InfraCommand** commands = (InfraCommand**)MALLOC(sizeof( InfraCommand* ));
            size_t commandCount = 1;

            void loadFrom(JsonObject& sourceObject) {
                if (commands) {
                    void* pointer = MALLOC(sizeof(InfraCommand));
                    InfraCommand* command = new (pointer) InfraCommand();
                    command->loadFrom(sourceObject);
                    commands[0] = command;
                }
                else {
                    log_e("alloc infrared commands failed");
                    while( true );
                }
            }

            void loadFrom(JsonArray& sourceArray) {
                if (commands != nullptr) free(commands);
                commandCount = sourceArray.size();
                commands = (InfraCommand**)MALLOC(sizeof( InfraCommand* ) * commandCount);

                if (commands) {
                    for (size_t i = 0; i < commandCount; i++) {
                        JsonObject sourceObject = sourceArray[i].as<JsonObject>();
                        
                        void* pointer = MALLOC(sizeof(InfraCommand));
                        InfraCommand* command = new (pointer) InfraCommand();
                        command->loadFrom(sourceObject);
                        commands[i] = command;
                        
                        // Process renaming property for compatibility reasons
                        if (sourceObject.containsKey("rn")) {
                            const char* renamed = sourceObject["rn"];
                            if (name != renamed && uiButton.isCreated())
                                uiButton.text(renamed);
                            strncpy( name, renamed, sizeof( name ) );
                        }
                    }
                }
                else {
                    log_e("alloc infrared commands failed");
                    while( true );
                }
            }
        };
    #else // NEW_HARDWARE_TAG
    #endif
#endif


#endif

