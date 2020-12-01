#include "TwoWireDevice.h"

#include "Arduino.h"
#include <stdio.h>

bool TwoWireDevice::begin()
{
    return _wire.begin();
};

/**
 *  @brief  read 8 bit value from device after writing cmd/reg
 *  @param  none
 *  @returns uint8_t - 8-bit value read
 */
uint8_t TwoWireDevice::read8()
{
	_wire.requestFrom(_i2caddr, (uint8_t) 0);
    return _wire.read();
};

/**
 *  @brief  write 8 bit value to device
 *  @param  uint8_t - value to be written
 *  @returns none
 */
void TwoWireDevice::write8(const uint8_t data)
{
	_wire.beginTransmission(_i2caddr);
	_wire.write(data);
	_wire.endTransmission();
};

void TwoWireDevice::write16_ML(const uint16_t data)
{
	_wire.beginTransmission(_i2caddr);
	_wire.write((uint8_t) data >> 8);
    _wire.write((uint8_t) data & 0xFF);
	_wire.endTransmission();
};

void TwoWireDevice::write16_LM(const uint16_t data)
{
	_wire.beginTransmission(_i2caddr);
    _wire.write((uint8_t) data & 0xFF);
	_wire.write((uint8_t) data >> 8);
	_wire.endTransmission();
};

/**
 *  @brief  read uint16_t MSB->LSB after writing a command/register to device
 *  @param  none
 *  @returns uint16_t - 16-bit value read
 */
uint16_t TwoWireDevice::read16_ML()
{
    _wire.requestFrom(_i2caddr, (uint8_t)2);
    return ((_wire.read() << 8) | _wire.read());
};

/**
 *  @brief  read uint16_t LSB->MSB after writing a command/register to device
 *  @param  none
 *  @returns uint16_t - 16-bit value read
 */
uint16_t TwoWireDevice::read16_LM()
{
    _wire.requestFrom(_i2caddr, (uint8_t)2);
    return ((_wire.read()) | _wire.read() << 8);
};

uint16_t TwoWireDevice::read24_ML()
{
    _wire.requestFrom(_i2caddr, (uint8_t)3);
    return ((_wire.read() << 16) | _wire.read() << 8 | _wire.read());
};

uint16_t TwoWireDevice::read24_LM()
{
    _wire.requestFrom(_i2caddr, (uint8_t)3);
    return ((_wire.read()) | _wire.read() << 8 | _wire.read() << 16);
};

void TwoWireDevice::read(uint8_t* buf, const uint8_t num)
{
	uint8_t pos = 0;

	//on arduino we need to read in 32 byte chunks
	while(pos < num)
    {
		uint8_t read_now = min(32, (num - pos));
		_wire.requestFrom(_i2caddr, read_now);

		for(int i=0; i<read_now; i++)
        {
			buf[pos] = _wire.read();
			pos++;
		};
	};
};

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
};

/**************************************************************************/
/*!
    @brief  read one byte of data from the specified register
    @param  reg the register to read
    @returns one byte of register data
*/
/**************************************************************************/
uint8_t TwoWireDevice::readreg8(const uint8_t reg)
{
    write8(reg);
    return read8();
};

/**
 *  @brief  write uint16_t from the specified register MSB, LSB
 *  @param  reg uint8_t - the register to read
 *  @returns uint16_t - register value
 */
void TwoWireDevice::writereg16_ML(const uint8_t reg, const uint16_t value)
{
    _wire.beginTransmission(_i2caddr);
    _wire.write(reg);
	_wire.write((uint8_t) (value >> 8));
 	_wire.write((uint8_t) value);
    _wire.endTransmission();	
};

/**
 *  @brief  read uint16_t from the specified register LSB, MSB
 *  @param  reg uint8_t - the register to read
 *  @returns uint16_t - register value
 */
uint16_t TwoWireDevice::readreg16_ML(const uint8_t reg)
{
    write8(reg);
    return read16_ML();
};


void TwoWireDevice::writereg16_LM(const uint8_t reg, const uint16_t value)
{
    _wire.beginTransmission(_i2caddr);
    _wire.write(reg);
 	_wire.write((uint8_t) value);			// LSB
	_wire.write((uint8_t) (value >> 8));	// LSB
    _wire.endTransmission();	
};

uint16_t TwoWireDevice::readreg16_LM(const uint8_t reg)
{
    // _wire.beginTransmission(_i2caddr);
    // _wire.write(reg);
    // _wire.endTransmission();
    write8(reg);
    
    // _wire.requestFrom(_i2caddr, (uint8_t)2);
    // return ((_wire.read()) | _wire.read() << 8);
    return read16_LM();
};

uint32_t TwoWireDevice::readreg24_LM(const uint8_t reg)
{
    write8(reg);
    return read24_LM();
};

uint32_t TwoWireDevice::readreg24_ML(const uint8_t reg)
{
    write8(reg);
    return read24_LM();
};

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
		};
	};
};

void TwoWireDevice::writereg(const uint8_t reg, const uint8_t *buf, const uint8_t num)
{
	_wire.beginTransmission(_i2caddr);
	_wire.write(reg);
	_wire.write(buf, num);
	_wire.endTransmission();
};

void TwoWireDevice::writereg(const uint8_t reg)
{
	_wire.beginTransmission(_i2caddr);
	_wire.endTransmission();
};
