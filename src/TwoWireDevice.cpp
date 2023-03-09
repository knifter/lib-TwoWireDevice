#include "TwoWireDevice.h"

#include "Arduino.h"
#include <stdio.h>

bool TwoWireDevice::begin(uint8_t address)
{
    if(address)
        _i2caddr = address;
    return _wire.begin();
};

// typedef enum {
//     I2C_ERROR_OK = 0,
//     I2C_ERROR_DEV,
//     I2C_ERROR_ACK = 2,
//     I2C_ERROR_TIMEOUT,
//     I2C_ERROR_BUS,
//     I2C_ERROR_BUSY,
//     I2C_ERROR_MEMORY,
//     I2C_ERROR_CONTINUE,
//     I2C_ERROR_NO_BEGIN
// } i2c_err_t;
uint8_t TwoWireDevice::last_error()
{
	return _last_error;
};
const char* TwoWireDevice::last_error_text()
{
	switch(_last_error)
	{
		case 0: return "OK";
		case 1: return "DEVICE";
    	case 2: return "No ACK from device.";
    	case 3: return "TIMEOUT";
    	case 4: return "BUS";
    	case 5: return "BUSY";
    	case 6: return "MEMORY";
    	case 7: return "OK CONTINUE";
    	case 8: return "No beginTransaction()";
		default: return "Unknown error";
	};
};

/**
 *  @brief  read 8 bit value from device after writing cmd/reg
 *  @param  none
 *  @returns uint8_t - 8-bit value read
 */
uint8_t TwoWireDevice::read8()
{
	_wire.requestFrom(_i2caddr, (uint8_t) 1);
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
	_last_error = _wire.endTransmission();
};

void TwoWireDevice::write16_ML(const uint16_t data)
{
	_wire.beginTransmission(_i2caddr);
	_wire.write(data >> 8);
    _wire.write(data & 0xFF);
	_last_error = _wire.endTransmission();
};

void TwoWireDevice::write16_LM(const uint16_t data)
{
	_wire.beginTransmission(_i2caddr);
    _wire.write(data & 0xFF);
	_wire.write(data >> 8);
	_last_error = _wire.endTransmission();
};

/**
 *  @brief  read uint16_t MSB->LSB after writing a command/register to device
 *  @param  none
 *  @returns uint16_t - 16-bit value read
 */
uint16_t TwoWireDevice::read16_ML()
{
    _wire.requestFrom(_i2caddr, (uint8_t) 2);
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

uint32_t TwoWireDevice::read24_ML()
{
    _wire.requestFrom(_i2caddr, (uint8_t)3);

	// union {
	// 	uint8_t bytes[4];
	// 	uint32_t value;
	// } res;
	// res.bytes[3] = 0;
	// res.bytes[2] = _wire.read();
	// res.bytes[1] = _wire.read();
	// res.bytes[0] = _wire.read();
	// return res.value;

    return ((_wire.read() << 16) | _wire.read() << 8 | _wire.read());
};

uint32_t TwoWireDevice::read24_LM()
{
    _wire.requestFrom(_i2caddr, (uint8_t)3);
    return ((_wire.read()) | _wire.read() << 8 | _wire.read() << 16);
};

int TwoWireDevice::read(uint8_t* buf, const uint8_t num)
{
	uint8_t cnt = 0;

	//on arduino we need to read in 32 byte chunks
	while(cnt < num)
    {
		uint8_t read_now = min(32, (num - cnt));
		// Serial.printf("requestFrom(%x, %d)\n", _i2caddr, read_now);
		uint8_t has_read = _wire.requestFrom(_i2caddr, read_now);
		if(has_read != read_now)
			return -1;

		for(int i=0; i<read_now; i++)
        {
			buf[cnt] = _wire.read();
			cnt++;
		};
	};
	return cnt;
};

void TwoWireDevice::write(const uint8_t* buf, const uint8_t num)
{
	uint8_t cnt = 0;

	_wire.beginTransmission(_i2caddr);

	//on arduino we need to read in 32 byte chunks
	while(cnt < num)
		_wire.write(buf[cnt++]);

	_last_error = _wire.endTransmission();

	return;
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
	_last_error = _wire.endTransmission();
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
    _last_error = _wire.endTransmission();	
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
    _last_error = _wire.endTransmission();	
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
    return read24_ML();
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
		_last_error = _wire.endTransmission();
		//TODO: check requestFrom result?
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
	_last_error = _wire.endTransmission();
};

void TwoWireDevice::writereg(const uint8_t reg)
{
	_wire.beginTransmission(_i2caddr);
	_last_error = _wire.endTransmission();
};
