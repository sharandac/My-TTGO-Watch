#if defined( ESP32 )

#include <Wire.h>
#include "CST816S.h"

// Write register values to chip
void CST816S::_writeReg(uint8_t reg, uint8_t *data, uint8_t len)
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
uint8_t CST816S::_readReg(uint8_t reg, uint8_t *data, uint8_t len)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(reg);
	_i2cPort->endTransmission();
	_i2cPort->requestFrom(_address, len);
	uint8_t index = 0;
	while (_i2cPort->available())
		data[index++] = _i2cPort->read();
	return 0;
}

bool CST816S::begin(TwoWire &port, uint8_t res, uint8_t INT, uint8_t addr)
{

	_i2cPort = &port;
	_address = addr;
	_res = res;
	_INT = INT;
	pinMode(_INT, INPUT_PULLUP);
	/* attachInterrupt(
		_INT, []
		{ isTouch = true },
		LOW); */
	setReset();

	_i2cPort->beginTransmission(_address);

	if (_i2cPort->endTransmission() != 0)
	{
		return false;
	}

	uint8_t temp[1];

	temp[0] = 0x00;
	_writeReg(DisAutoSleep, temp, 1); //默认为0，使能自动进入低功耗模式

	temp[0] = 0x02;
	_writeReg(NorScanPer, temp, 1); //设置报点率

	temp[0] = 0x60;				//报点：0x60  手势：0X11  报点加手势：0X71
	_writeReg(IrqCtl, temp, 1); //设置模式 报点/手势

	temp[0] = 0x05;				   //单位1S  为0时不启用功能  默认5
	_writeReg(AutoReset, temp, 1); //设置自动复位时间  X秒内有触摸但无手势时，自动复位

	temp[0] = 0x10;					   //单位1S  为0时不启用功能  默认10
	_writeReg(LongPressTime, temp, 1); //设置自动复位时间  长按X秒自动复位

	temp[0] = 0x1E;					   //单位0.1mS
	_writeReg(IrqPluseWidth, temp, 1); //设置中断低脉冲输出宽度

/* 	temp[0] = 0x30;
	_writeReg(LpScanTH, temp, 1); //设置低功耗扫描唤醒门限
	temp[0] = 0x01;
	_writeReg(LpScanWin, temp, 1); //设置低功耗扫描量程*/
	/* temp[0] = 0x50;
	_writeReg(LpScanFreq, temp, 1); //设置低功耗扫描频率 
	
	temp[0] = 0x80;
	_writeReg(LpScanIdac, temp, 1); //设置低功耗扫描电流 

	temp[0] = 0x01;
	_writeReg(AutoSleepTime, temp, 1); //设置1S进入低功耗

	_readReg(0x00, Touch_Data, 7);*/

	return true;
}

// Reset the chip
void CST816S::setReset()
{
	pinMode(_res, OUTPUT);
	digitalWrite(_res, LOW);
	delay(10);
	digitalWrite(_res, HIGH);
	delay(50);
}

// Set I2C Address if different then default.
void CST816S::setADDR(uint8_t b)
{
	_address = b;
}

bool CST816S::TouchInt(void)
{
	if (!digitalRead(_INT))
	{
		_readReg(0x00, Touch_Data, 7);
		return true;
	}
	else
		return false;
}

void CST816S::ReadTouch(void)
{
	_readReg(0x00, Touch_Data, 7);
}

/* 
 * 按下按钮返回：0x01
 */
uint8_t CST816S::getTouchType(void)
{
	return Touch_Data[3] >> 7;
}

uint16_t CST816S::getX(void)
{
	return ((uint16_t)(Touch_Data[3] & 0x0F) << 8) + (uint16_t)Touch_Data[4];
}

uint16_t CST816S::getY(void)
{
	return ((uint16_t)(Touch_Data[5] & 0x0F) << 8) + (uint16_t)Touch_Data[6];
}

#endif