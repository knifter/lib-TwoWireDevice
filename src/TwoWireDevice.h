
#ifndef __TWOWIREDEVICE_H
#define __TWOWIREDEVICE_H

#include <stdint.h>
#include <Wire.h>
#include <Arduino.h>

/* Adafruit usesd _LE (litte-endian) suffix for _LM
 */

class TwoWireDevice
{
    public:
        TwoWireDevice(TwoWire& wire, const uint8_t addr) : _i2caddr(addr), _wire(wire) {};
        TwoWireDevice(const uint8_t addr) : _i2caddr(addr), _wire(Wire) {};

        bool begin(uint8_t address = 0);
        uint8_t last_error();
        const char* last_error_text();

    protected:
        // Read/Write plain data / Building blocks
        int       read(uint8_t* buf, const uint8_t num);
        void	  write(const uint8_t* buf, const uint8_t num);
        void 	  readreg(const uint8_t reg, uint8_t *buf, const uint8_t num);
        void 	  writereg(const uint8_t reg, const uint8_t *buf, const uint8_t num);
        void 	  writereg(const uint8_t reg);

        // 8-bit
        uint8_t   read8();
        void      write8(const uint8_t data);

        uint8_t   readreg8(const uint8_t reg);
        void      writereg8(const uint8_t reg, const uint8_t value);

        // 16-bit
        uint16_t  read16_ML();
        void      write16_ML(const uint16_t data);

        uint16_t  read16_LM();
        void      write16_LM(const uint16_t data);

        uint16_t  readreg16_ML(const uint8_t reg);
        void      writereg16_ML(const uint8_t reg, const uint16_t value);

        uint16_t  readreg16_LM(const uint8_t reg);
        void      writereg16_LM(const uint8_t reg, const uint16_t value);

        // 24-bit data/registers
        uint32_t  read24_ML();
        // 	void      write24_ML(const uint32_t data);

        uint32_t  read24_LM();
        // 	void	  write24_LM(const uint32_t data);

        uint32_t  readreg24_LM(const uint8_t reg);
        //         void      writereg24_ML(const uint8_t reg, const uint32_t value);

        uint32_t  readreg24_ML(const uint8_t reg);
        //         void      writereg24_ML(const uint8_t reg, const uint32_t value);

        // Members
        uint8_t _i2caddr;
        uint8_t _last_error;
        TwoWire& _wire = Wire;

    public: // more readable errors if public
        // TwoWireDevices are by default not copyable
        TwoWireDevice(const TwoWireDevice&) = delete;
        TwoWireDevice& operator=(const TwoWireDevice&) = delete;
};

#endif //__TWOWIREDEVICE_H
