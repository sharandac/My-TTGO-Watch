#if defined( ESP32 )

#ifndef __CST816S_H__
#define __CST816S_H__

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
//      Bit
#define EnConLR _BV(2)	// Can continuously slide left and right
#define EnConUD _BV(1)	// Enable continuous sliding up and down
#define EnDClick _BV(0) // Enable double-click action
//

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
//      Bit
#define EnTest _BV(7)	// Interrupt the pin test, and automatically send out low pulse periodically after being enabled.
#define EnTouch _BV(6)	// When a touch is detected, periodically send out low pulses
#define EnChange _BV(5) // When a touch state change is detected, a low pulse is sent.
#define EnMotion _BV(4) // When a gesture is detected, a low pulse is emitted.
#define OnceWLP _BV(0)	// The long press gesture only sends out a low pulse signal.
//

#define AutoReset 0XFB
#define LongPressTime 0XFC
#define IOCtl 0XFD
#define DisAutoSleep 0XFE

#define _BV(b) (1UL << (b))

class CST816S_Class
{
	enum
	{
		No_Gesture = 0x00,
		Slide_Up,
		Slide_Down,
		Slide_Left,
		Slide_Right,
		Click_On,
		Double_Click,
		Press
	}; // GestureID
public:
	bool begin(TwoWire &port, uint8_t res, uint8_t _int = -1, uint8_t addr = CTP_SLAVER_ADDR);
	void setReset();
	void setADDR(uint8_t b);
	bool read(void);
	uint16_t getX(void);
	uint16_t getY(void);

	void TouchInt(void);

	uint8_t CheckID(void);
	uint8_t getTouchType(void);
	void setAutoLowPower(bool en = false);
	void setTouchInt(bool en = false);
	void setGesture(bool en = false);
	void setGestureCalibration(uint8_t data);
private:
	void _writeReg(uint8_t reg, uint8_t data);
	void _writeReg(uint8_t reg, uint8_t *data, uint8_t len);
	uint8_t _readReg(uint8_t reg, uint8_t *data, uint8_t len);
	uint8_t _address;
	bool _init = false;
	TwoWire *_i2cPort;
	uint8_t _res = -1;
	uint8_t _int = -1;
	uint8_t Touch_Data[10];
};

#endif

#endif // ESP32