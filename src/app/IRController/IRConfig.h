#ifndef IR_CONFIG_H
    #define IR_CONFIG_H

    #include "config.h"
    #include "quickglui/quickglui.h"
    #include "IRButton.h"

    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            #define IR_BUTTONS_START 16
            #define IR_BUTTONS_TRESHOLD 8

            class IRConfig : public BaseJsonConfig
            {
                public:
                IRConfig();

                InfraButton* add(size_t page, const char* name);
                void del(size_t page, const char* name);
                InfraButton* get(size_t page, const char* name);

                InfraButton* get(int id) { return buttons[id]; }
                int totalCount() { return buttonCount; }

                void sendListNames(BluetoothJsonResponse& target);
                void sendButtonEdit(BluetoothJsonResponse& target, size_t page, const char* name);

                public:
                int defBtnWidth = 95;
                int defBtnHeight = 33;
                int defSpacing = 3;
                size_t pageCount = 1;

                protected:
                virtual bool onSave(JsonDocument& document);
                virtual bool onLoad(JsonDocument& document);
                virtual bool onDefault( void );
                virtual size_t getJsonBufferSize() { return 48000; }

                protected:
                InfraButton** buttons = nullptr;
                size_t buttonCount = 0;
            };
        #elif defined( LILYGO_WATCH_2021 )  
        #elif defined( WT32_SC01 )
        #else
            /**
             * NEW_HARDWARE_TAG or not defined
             */
            #warning "No destination hardware defined"
        #endif
    #endif
#endif

