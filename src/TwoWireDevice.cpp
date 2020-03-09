#include "TwoWireDevice.h"

#include "Arduino.h"
#include <stdio.h>

bool TwoWireDevice::begin()
{
    _wire.begin();
    return true;
}

uint8_t TwoWireDevice::read8()
{
	_wire.requestFrom(_i2caddr, (uint8_t) 0);
    return _wire.read();
}

void TwoWireDevice::write8(const uint8_t data)
{
	_wire.beginTransmission(_i2caddr);
	_wire.write(data);
	_wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  write one byte of data to the specified register
    @param  reg the register to write to
    @param  value the value to write
*/
/**************************************************************************/
void TwoWireDevice::writereg8(const uint8_t reg, const uint8_t value)
{
	_wire.beginTransmission(_i2caddr);
	_wire.write(reg);
	_wire.write(value);
	_wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  read one byte of data from the specified register
    @param  reg the register to read
    @returns one byte of register data
*/
/**************************************************************************/
uint8_t TwoWireDevice::readreg8(const uint8_t reg)
{
    _wire.beginTransmission(_i2caddr);
    _wire.write(reg);
    _wire.endTransmission();
    _wire.requestFrom(_i2caddr, (uint8_t)1);
    return _wire.read();
}

/**
 *  @brief  read uint16 from the specified register
 *  @param  reg uint8 - the register to read
 *  @returns uint16 - register value
 */
uint16_t TwoWireDevice::readreg16(const uint8_t reg)
{
    _wire.beginTransmission(_i2caddr);
    _wire.write(reg);
    _wire.endTransmission();
    _wire.requestFrom(_i2caddr, (uint8_t)2);
    return ((_wire.read() << 8) | _wire.read());
}

void TwoWireDevice::writereg16(const uint8_t reg, const uint16_t value)
{
    _wire.beginTransmission(_i2caddr);
    _wire.write(reg);
	_wire.write((uint8_t) (value >> 8));
 	_wire.write((uint8_t) value);
    _wire.endTransmission();	
}

uint16_t TwoWireDevice::readreg16_LE(const uint8_t reg)
{
    _wire.beginTransmission(_i2caddr);
    _wire.write(reg);
    _wire.endTransmission();
    _wire.requestFrom(_i2caddr, (uint8_t)2);
    return ((_wire.read()) | _wire.read() << 8);
}

void TwoWireDevice::writereg16_LE(const uint8_t reg, const uint16_t value)
{
	_wire.beginTransmission(_i2caddr);
	_wire.write(reg);
	_wire.write(value & 0xFF);
    _wire.write((value & 0xFF00) >> 8);
	_wire.endTransmission();
}

uint32_t TwoWireDevice::readreg24(const uint8_t reg)
{
    uint32_t value;

    _wire.beginTransmission(_i2caddr);
    _wire.write(reg);
    _wire.endTransmission();
    _wire.requestFrom(_i2caddr, (uint8_t)3);

    value = _wire.read();
    value <<= 8;
    value |= _wire.read();
    value <<= 8;
    value |= _wire.read();

    return value;
}

void TwoWireDevice::readreg(const uint8_t reg, uint8_t *buf, const uint8_t num)
{
	uint8_t pos = 0;

	//on arduino we need to read in 32 byte chunks
	while(pos < num)
    {
		uint8_t read_now = min(32, (num - pos));
		_wire.beginTransmission(_i2caddr);
		_wire.write(reg + pos);
		_wire.endTransmission();
		_wire.requestFrom(_i2caddr, read_now);

		for(int i=0; i<read_now; i++)
        {
			buf[pos] = _wire.read();
			pos++;
		}
	}
}

void TwoWireDevice::writereg(const uint8_t reg, const uint8_t *buf, const uint8_t num)
{
	_wire.beginTransmission(_i2caddr);
	_wire.write(reg);
	_wire.write(buf, num);
	_wire.endTransmission();
}

