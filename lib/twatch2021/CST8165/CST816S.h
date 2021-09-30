#if defined( ESP32 )
    #ifndef _CST8165_H
        #define _CST8165_H
        #include <Wire.h>

        #define CTP_SLAVER_ADDR 0x15

        #define GestureID 0x01
        #define FingerNum 0x02
        #define XposH 0x03
        #define XposL 0x04
        #define YposH 0x05
        #define YposL 0x06

        #define BPC0H 0XB0
        #define BPC0L 0XB1
        #define BPC1H 0XB2
        #define BPC1L 0XB3

        #define ChipID 0XA7
        #define ProjID 0XA8
        #define FwVersion 0XA9

        #define MotionMask 0XEC
        #define IrqPluseWidth 0XED
        #define NorScanPer 0XEE
        #define MotionSlAngle 0XEF

        #define LpScanRaw1H 0XF0
        #define LpScanRaw1L 0XF1
        #define LpScanRaw2H 0XF2
        #define LpScanRaw2L 0XF3
        #define LpScanTH 0XF5
        #define LpScanWin 0XF6
        #define LpScanFreq 0XF7
        #define LpScanIdac 0XF8
        #define AutoSleepTime 0XF9
        #define IrqCtl 0XFA
        #define AutoReset 0XFB
        #define LongPressTime 0XFC
        #define IOCtl 0XFD
        #define DisAutoSleep 0XFE

        enum {
            No_Gesture = 0x00,
            Slide_Up,
            Slide_Down,
            Slide_Left,
            Slide_Right,
            Click_On,
            Double_Click,
            Press
        };//GestureID



        class CST816S
        {
        public:
            bool begin(TwoWire &port, uint8_t res, uint8_t INT, uint8_t addr = CTP_SLAVER_ADDR);
            void setReset();
            void setADDR(uint8_t b);
            void cst816s_deep_sleep(void);
            void _writeReg(uint8_t reg, uint8_t *data, uint8_t len);
            uint8_t _readReg(uint8_t reg, uint8_t *data, uint8_t len);
            void ReadTouch(void);
            bool TouchInt(void);
            uint8_t getTouchType(void);
            uint16_t getX(void);
            uint16_t getY(void);

        private:
            uint8_t _address;
            bool _init = false;
            TwoWire *_i2cPort;
            uint8_t _res = -1;
            uint8_t _INT = -1;
            uint8_t Touch_Data[10];
        };
    #endif
#endif