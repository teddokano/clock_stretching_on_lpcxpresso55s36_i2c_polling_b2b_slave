# I²C target device emulator with variable clock stretching setting

## What is this?
An I²C target device emulator on LPC55S36-EVK to perform clock stretching.   

The target device emulates simple memory device.  
It is just 256 bytes RAM which can be accessed via I²C.  

## Purpose of this code (Why this code had been written?)
"Clock stretch" is an optional feature of the I²C bus interface. 
It is the future to controller wait from target device. 
It is done by holding SCL (serial clock line) LOW until next data send/receive ready.  
To test controller which supports the clock stretching, this target will do the clock stretching with specified time. 

## Environment for build
Zipped project can be imported into MCUXpresso-IDE.  
Expecting the LPCXpresso55s36 SDK 2.15.000 installed in the IDE.  

## Hardware setting
**Supply / Debug interface:**  
- J1 (Link2) for PC connection  

**Jumper seting:**  
- Short JP52 1-2  

**Signal pins:**  
- SCL = J122-12  
- SDA = J122-6  
- GND = J92-14   

## Basic operation: Memory access
Memory access is performed with main target address (**MainADDR**, default: 0b1011010 = 0x5A in 7 bit format). This main target address can be changed on `I2C_MASTER_SLAVE_ADDR_7BIT` in **source/setting.h** file. 

It has 256 bytes internal memory space which can be accessed from I2C.  
The data access (read and write) is done sequencially from specified memory offset.  
 
**Memory writing** can be done by following I²C transfer:  
`[START]-[MainADDR+W]-[Offset]-[Data0]-[Data1]-…-[DataN]-[STOP]`  
The first byte after the [ADDR+W] is **offset** of start data writing. Data length for writing has no limitation. If it goes over memory size, the data write will be wrap-around.  
 
**Memory reading** can be done by:  
`[START]-[MainADDR+W]-[Offset]-[ReSTART]-[MainADDR+R]-[Data0]-[Data1]-…[DataN]-[STOP]`  
The first byte after the [ADDR+W] is **offset** of start data reading. Data length for reading has no limitation. If it goes over memory size, the data read will be wrap-around.  
The [ReSTART] can be replaced by [STOP]-[START].  
If the memory offset is not specified, the next byte of previously accessed.  
 
## Extended operation: Accessing clock stretch setting
Clock stretch duration setting can be done by accessing target sub-address (**SubADDR**, default: 0b1011011 = 0x5B in 7 bit format). This target sub- address can be changed on `I2C_SUBADDR0_7BIT` in **source/setting.h** file.  

3 types of clock stretch duration can be set independently.  
- Type 0 : **STRETCH_FOR_ADDRESS** = Stretch on ACK after target address
- Type 1 : **STRETCH_FOR_WRITE** = Stretch on ACK after writing data
- Type 2 : **STRETCH_FOR_READ** = Stretch before Next byte read

The stretch dulation can be set in **micro-second** from 0 to 2^32 (32 bits unsigned int).  
The setting can be done in next format.  
`[START]-[SubADDR+W]-[Type]-[Data0]-[Data1]-[Data2]-[Data3]-[STOP]`
 
The data order is in little-endian. LS-Byte (Least Significant byte) go first on the transfer.  
User don't need to transfer all 4 bytes in each transfer. If the transfer doesn't have missing bytes, it will be treated as `0`. 

### Examples
To set 100 (0x64) micro-seconds for STRETCH_FOR_ADDRESS (Type = 0x00):  
`[START]-[0x5B+W]-[0x00]-[0x64]-[STOP]`  
There are no Data1..Data3 but those are treated as 0x00. So 0x00000064 is set for stretch.  

To set 1000 (0x3E8) micro-seconds for STRETCH_FOR_READ (Type = 0x02):  
`[START]-[0x5B+W]-[0x02]-[0xE8]-[0x03]-[STOP]`  
There are no Data2..Data3 but those are treated as 0x00. So 0x000003E8 is set for stretch.  

## Sample code on I²C controller
To operate this target device, any I²C can be used.  
The operation can be done by just implementing transfer described above.  

But for just in case, to show complete idea of usage, sample code is available in next URL.  
The sample code is for FRDM-K64F, using Mbed SDK.  
[https://github.com/teddokano/i2c_clock_stretch_test_controller_mbed](https://github.com/teddokano/i2c_clock_stretch_test_controller_mbed)
