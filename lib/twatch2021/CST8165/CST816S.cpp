#if defined( ESP32 )

#include <Wire.h>
#include "CST816S.h"

void CST816S_Class::_writeReg(uint8_t reg, uint8_t data)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(reg);
	_i2cPort->write(data);
	_i2cPort->endTransmission();
}

// Write register values to chip
void CST816S_Class::_writeReg(uint8_t reg, uint8_t *data, uint8_t len)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(reg);
	for (uint8_t i = 0; i < len; i++)
	{
		_i2cPort->write(data[i]);
	}
	_i2cPort->endTransmission();
}

// read register values to chip
uint8_t CST816S_Class::_readReg(uint8_t reg, uint8_t *data, uint8_t len)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(reg);
	_i2cPort->endTransmission();
	_i2cPort->requestFrom(_address, len);
	uint8_t index = 0;
	while (_i2cPort->available())
	// while (index < len)
		data[index++] = _i2cPort->read();
	return 0;
}

bool CST816S_Class::begin(TwoWire &port, uint8_t res, uint8_t _int, uint8_t addr)
{
	_i2cPort = &port;
	_address = addr;
	_res = res;
	_int = _int;
	if (_int != 0)
		pinMode(_int, INPUT_PULLUP);
	/* attachInterrupt(_INT, []{ isTouch = true },LOW); */
	setReset();
	_i2cPort->setClock(100000);
	_i2cPort->beginTransmission(_address);

	if (_i2cPort->endTransmission() != 0)
	{
		printf("CST816S NO Found!\n");
		return false;
	}

	_writeReg(DisAutoSleep, 0x01); //默认为0，使能自动进入低功耗模式

	_writeReg(NorScanPer, 0x01); //设置报点率

	//报点：0x60  手势：0X11  报点加手势：0X71
	_writeReg(IrqCtl, 0x60);		//设置模式 报点/手势
									//单位1S  为0时不启用功能  默认5
	_writeReg(AutoReset, 0x0);		//设置自动复位时间  X秒内有触摸但无手势时，自动复位
									//单位1S  为0时不启用功能  默认10
	_writeReg(LongPressTime, 0x10); //设置自动复位时间  长按X秒自动复位
									//单位0.1mS
	_writeReg(IrqPluseWidth, 0x02); //设置中断低脉冲输出宽度

	/* 	data = 0x30;
	_writeReg(LpScanTH, &data, 1); //设置低功耗扫描唤醒门限
	data = 0x01;
	_writeReg(LpScanWin, &data, 1); //设置低功耗扫描量程*/
	/* data = 0x50;
	_writeReg(LpScanFreq, &data, 1); //设置低功耗扫描频率 */

	/* 	data = 0x80;
	_writeReg(LpScanIdac, &data, 1); //设置低功耗扫描电流
	data = 0x01;
	_writeReg(AutoSleepTime, &data, 1); //设置1S进入低功耗 */

	_readReg(0x00, Touch_Data, 7);

	return true;
}

// Reset the chip
void CST816S_Class::setReset()
{
	if (_res != -1)
	{
		pinMode(_res, OUTPUT);
		digitalWrite(_res, LOW);
		delay(10);
		digitalWrite(_res, HIGH);
		delay(50);
	}
	else
	{
		_writeReg(IOCtl, _BV(2));
		delay(50);
	}
}

// Set I2C Address if different then default.
void CST816S_Class::setADDR(uint8_t b)
{
	_address = b;
}

bool CST816S_Class::read(void)
{
	_readReg(0x00, Touch_Data, 7);
	if (Touch_Data[3] >> 7)
		return true;
	else
		return false;
}

void CST816S_Class::TouchInt(void)
{
	_readReg(0x00, Touch_Data, 7);
}

uint8_t CST816S_Class::CheckID(void)
{
	uint8_t data;
	_readReg(ChipID, &data, 1);
	return data;
}

uint8_t CST816S_Class::getTouchType(void)
{
	return Touch_Data[1] >> 7;
}

uint16_t CST816S_Class::getX(void)
{
	return ((uint16_t)(Touch_Data[3] & 0x0F) << 8) + (uint16_t)Touch_Data[4];
}

uint16_t CST816S_Class::getY(void)
{
	return ((uint16_t)(Touch_Data[5] & 0x0F) << 8) + (uint16_t)Touch_Data[6];
}

void CST816S_Class::setAutoLowPower(bool en)
{
	_writeReg(DisAutoSleep, en); //默认为0，使能自动进入低功耗模式
}

// Does not generate a pull-down signal
void CST816S_Class::setTouchInt(bool en)
{
	_writeReg(IrqCtl, en ? 0x60 : 0x00); //设置模式 报点/手势
}

void CST816S_Class::setGesture(bool en)
{
	_writeReg(MotionMask, en ? (EnConLR | EnConUD | EnDClick) : 0x00);
}

// Gesture detection sliding zone angle control. Angle=tan(c)*10 c is the angle based on the positive x-axis direction.
void CST816S_Class::setGestureCalibration(uint8_t data)
{
	_writeReg(MotionSlAngle, data);
}

#endif // ESP32