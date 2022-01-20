# Introduction
Arduino/ESP32 library to wrap TwoWire device classes. 

While writing several supoport libraries for I2C/TwoWire devices it became cumbersome to keep implementing the read/write functions and sorting out the byte order for each device. Also most libraries I found as an example all assumed that 'Wire' was going to be used as the only TwoWire bus. I wanted to be able to support several busses and several devices on each bus. On top of that each library was randomly reconfiguring io-pins, bus speeds etc which would make them conflict each other.
My view on this is that a developer should first configure the bus, speed, pins etc and then instantiate devices on them.
That is what this libary supports.

It's major features are:
 * A common place to store the read/write address/register functions for several bit widths (8/16/24) so libraries can share them.
 * These devices all supprt which 'Wire' device you wish to use and on which address. Therefor you can easily add multiple sensors on one or more buses and make multiple instances of this library.

There is a less sophisticated twin: [lib-SPIDevice](https://github.com/knifter/lib-SPIDevice)

# Usage
## Code
### Declare
Inherited your devices from TwoWireDevice:
```
#define MYDEVICE_ADDRESS_DEFAULT    0x00

class MyDevice : public TwoWireDevice 
{
	public:
		//con-/destructors
		MyDevice(TwoWire& wire, const uint8_t addr = MYDEVICE_ADDRESS_DEFAULT)  : TwoWireDevice(wire, addr) {};
		MyDevice(const uint8_t addr = MYDEVICE_ADDRESS_DEFAULT) : TwoWireDevice(addr) {};
    		~MyDevice() {};
};
```

### Read / Write to device
Then use the TwoWireDevice's protected functions to read/write from/to the device's :
  * `read()` / `write()`, an arbitrary number of bytes
  * `read8()` / `write8()`, one byte

Or use the register funtions to read/write register based devices:
 * `readreg8`/ `writereg8`
 * `readreg16` / `writereg16`
 * `readreg24` / `writereg24`

Of which each of them is available in MSB>LSB or LSB>MSB mode:
 * `readreg8_ML()`, MSB left, LSB right
 * `readreg8_LM()`, LSB left, MSB right

### Check results
After each call to the Wire library the result is stored and can be retreived with the `last_error()` method, or a string can be obtained with `last_error_text()` for debugging purposes.

## Copy constructor
Personally I find it good practice to, at first, disable copy constructors and assignment operators until I've thought about them. In TwoWireDevices this is seldom something you want to support, I guess.. So I've disabled them in TwoWireDevice.h. Implement your own in your (inherited) device to re-enable them after you have done the thinking ;)
```
    private:
		    TwoWireDevice(const TwoWireDevice&);
		    TwoWireDevice& operator=(const TwoWireDevice&);
```

## PlatformIO
I'm an advocate of [Platform-IO](https://platformio.org/platformio-ide). To use this library with your PlatformIO project, add this to you platformio.ini environment:
``` 
lib_deps =
  https://github.com/knifter/lib-TwoWireDevice
```

## Examples
Some example devices are between my github repositories, like:
#### Environmental sensors
 * [x] [SHT30/31/35 Temperature/Humidity sensor](https://github.com/knifter/lib-SHT3x)
 * [ ] [SHT40/41/45 Temperature/Humidity sensor](https://github.com/knifter/lib-SHT4x)
 * [x] [CCS811 Gas Sensor](https://github.com/knifter/lib-CCS811)
 * [x] [BME280 Humidity/Pressure/Temperature Environmental Sensor](https://github.com/knifter/lib-BME280)

#### Voltage/Current Sensors
 * [x] [INA233 Bi-directional Voltage/Current/Power/Energy monitor](https://github.com/knifter/lib-INA233)
 * [x] [INA219 Voltage/Current Sensor](https://github.com/knifter/lib-INA219)
#### Audio
 * [x] [STA320 Full-Digital Class-D Amplifier](https://github.com/knifter/lib-STA320)
 * [x] [WM8805 SPDIF Transceiver](https://github.com/knifter/lib-WM8805)

#### Other
 * [x] [CPMS12 Compass Module with Gyro sensor-fusion](https://github.com/knifter/lib-CMPS12)
 * [x] [PCF8574 Remote 8-bit IO-Expander](https://github.com/knifter/lib-PCF8574)
 * [x] [MCP23008 Remote 8-bit IO-Expander](https://github.com/knifter/lib-MCP23008)

#### TODO
 * [ ] [MCP2515 CAN Controller](https://github.com/knifter/lib-MCP2515)
 
