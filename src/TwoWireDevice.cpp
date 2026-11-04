#include "TwoWireDevice.h"

#include "Arduino.h"
#include <stdio.h>
#if defined(ARDUINO_ARCH_ESP32)
    #include "esp32-hal-i2c.h"
#endif

void TwoWireDevice::_beginTransmission()
{
    _wire.beginTransmission(_i2caddr);
};

uint8_t TwoWireDevice::_requestFrom(uint8_t len)
{
		uint8_t got = _wire.requestFrom(_i2caddr, len);
    _last_error = (got == len) ? ERROR_OK : ERROR_SHORTREAD;
    return got;
};

bool TwoWireDevice::begin(uint8_t address)
{
    if(address)
        _i2caddr = address;

#if defined(ARDUINO_ARCH_ESP32)
    // The application must start the bus (with its pins) before the devices
    if(!i2cIsInit(_wire.getBusNum()))
    {
        _last_error = ERROR_BUS_NO_BEGIN;
        return false;
    };
#endif

    // Lower the bus clock before any traffic, so this device never sees a clock it can't follow
    if(_wire.getClock() > max_clock())
        _wire.setClock(max_clock());
    return true;
};

bool TwoWireDevice::ping()
{
    _beginTransmission();
	_last_error = _wire.endTransmission();
    if(_last_error == 0)
		return true;
	// Serial.printf("0x%02x : _last_error = %d (%s)\n", _i2caddr, _last_error, last_error_text());
	return false;
};

TwoWireDevice::error_t TwoWireDevice::last_error()
{
	return static_cast<error_t>(_last_error);
};
const char* TwoWireDevice::last_error_text()
{
	switch(_last_error)
	{
		case TwoWireDevice::ERROR_OK: 			return "OK";
		case TwoWireDevice::ERROR_DATALONG: 	return "Data too long";
    	case TwoWireDevice::ERROR_NACK_ADDR: 	return "NACK on address";
    	case TwoWireDevice::ERROR_NACK_DATA: 	return "NACK on data";
    	case TwoWireDevice::ERROR_OTHER: 		return "BUS/Other";
    	case TwoWireDevice::ERROR_TIMEOUT: 		return "Timeout";
    	case TwoWireDevice::ERROR_MEMORY: 		return "MEMORY";
    	case TwoWireDevice::ERROR_CONTINUE: 	return "OK CONTINUE";
    	case TwoWireDevice::ERROR_NO_BEGIN: 	return "No beginTransaction()";
    	case TwoWireDevice::ERROR_SHORTREAD: 	return "Short read";
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
	_requestFrom((uint8_t) 1);
    return _wire.read();
};

/**
 *  @brief  write 8 bit value to device
 *  @param  uint8_t - value to be written
 *  @returns none
 */
void TwoWireDevice::write8(const uint8_t data)
{
	_beginTransmission();
	_wire.write(data);
	_last_error = _wire.endTransmission();
};

void TwoWireDevice::write16_ML(const uint16_t data)
{
	_beginTransmission();
	_wire.write(data >> 8);
    _wire.write(data & 0xFF);
	_last_error = _wire.endTransmission();
};

void TwoWireDevice::write16_LM(const uint16_t data)
{
	_beginTransmission();
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
    _requestFrom((uint8_t) 2);
    uint8_t msb = _wire.read();
    uint8_t lsb = _wire.read();
    return (((uint16_t)msb << 8) | lsb);
};

/**
 *  @brief  read uint16_t LSB->MSB after writing a command/register to device
 *  @param  none
 *  @returns uint16_t - 16-bit value read
 */
uint16_t TwoWireDevice::read16_LM()
{
    _requestFrom((uint8_t)2);
    uint8_t lsb = _wire.read();
    uint8_t msb = _wire.read();
    return (lsb | (uint16_t)msb << 8);
};

uint32_t TwoWireDevice::read24_ML()
{
    _requestFrom((uint8_t)3);

	// union {
	// 	uint8_t bytes[4];
	// 	uint32_t value;
	// } res;
	// res.bytes[3] = 0;
	// res.bytes[2] = _wire.read();
	// res.bytes[1] = _wire.read();
	// res.bytes[0] = _wire.read();
	// return res.value;

    uint8_t b2 = _wire.read();
    uint8_t b1 = _wire.read();
    uint8_t b0 = _wire.read();
    return (((uint32_t)b2 << 16) | (uint32_t)b1 << 8 | b0);
};

uint32_t TwoWireDevice::read24_LM()
{
    _requestFrom((uint8_t)3);
    uint8_t b0 = _wire.read();
    uint8_t b1 = _wire.read();
    uint8_t b2 = _wire.read();
    return (b0 | (uint32_t)b1 << 8 | (uint32_t)b2 << 16);
};

int TwoWireDevice::read(uint8_t* buf, const uint8_t num)
{
	uint8_t cnt = 0;

	//on arduino we need to read in 32 byte chunks
	while(cnt < num)
    {
		uint8_t read_now = min(32, (num - cnt));
		// Serial.printf("requestFrom(%x, %d)\n", _i2caddr, read_now);
		uint8_t has_read = _requestFrom(read_now);
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

	_beginTransmission();

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
	_beginTransmission();
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
    if(_last_error)
        return 0xFF;
    return read8();
};

/**
 *  @brief  write uint16_t from the specified register MSB, LSB
 *  @param  reg uint8_t - the register to read
 *  @returns uint16_t - register value
 */
void TwoWireDevice::writereg16_ML(const uint8_t reg, const uint16_t value)
{
    _beginTransmission();
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
    if(_last_error)
        return 0xFFFF;
    return read16_ML();
};


void TwoWireDevice::writereg16_LM(const uint8_t reg, const uint16_t value)
{
    _beginTransmission();
    _wire.write(reg);
 	_wire.write((uint8_t) value);			// LSB
	_wire.write((uint8_t) (value >> 8));	// LSB
    _last_error = _wire.endTransmission();	
};

uint16_t TwoWireDevice::readreg16_LM(const uint8_t reg)
{
    // _beginTransmission();
    // _wire.write(reg);
    // _wire.endTransmission();
    write8(reg);
    if(_last_error)
        return 0xFFFF;

    // _requestFrom((uint8_t)2);
    // return ((_wire.read()) | _wire.read() << 8);
    return read16_LM();
};

uint32_t TwoWireDevice::readreg24_LM(const uint8_t reg)
{
    write8(reg);
    if(_last_error)
        return 0xFFFFFF;
    return read24_LM();
};

uint32_t TwoWireDevice::readreg24_ML(const uint8_t reg)
{
    write8(reg);
    if(_last_error)
        return 0xFFFFFF;
    return read24_ML();
};

void TwoWireDevice::readreg(const uint8_t reg, uint8_t *buf, const uint8_t num)
{
	uint8_t pos = 0;

	//on arduino we need to read in 32 byte chunks
	while(pos < num)
    {
		uint8_t read_now = min(32, (num - pos));
		_beginTransmission();
		_wire.write(reg + pos);
		_last_error = _wire.endTransmission();
		if(_last_error)
			return;
		//TODO: check requestFrom result?
		_requestFrom(read_now);

		for(int i=0; i<read_now; i++)
        {
			buf[pos] = _wire.read();
			pos++;
		};
	};
};

void TwoWireDevice::writereg(const uint8_t reg, const uint8_t *buf, const uint8_t num)
{
	_beginTransmission();
	_wire.write(reg);
	_wire.write(buf, num);
	_last_error = _wire.endTransmission();
};

void TwoWireDevice::writereg(const uint8_t reg)
{
	_beginTransmission();
	_last_error = _wire.endTransmission();
};
