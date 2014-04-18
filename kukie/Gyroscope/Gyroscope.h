/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef Gyroscope_h
#define Gyroscope_h

//The Wire library is used for I2C communication
#include "Arduino.h"
#include <Wire.h>

//This is a list of registers in the ITG-3200. Registers are parameters that determine how the sensor will behave, or they can hold data that represent the
//sensors current status.
//To learn more about the registers on the ITG-3200, download and read the datasheet.
#define WHO_AM_I 0x00
#define SMPLRT_DIV 0x15
#define DLPF_FS 0x16
#define GYRO_XOUT_H 0x1D
#define GYRO_XOUT_L 0x1E
#define GYRO_YOUT_H 0x1F
#define GYRO_YOUT_L 0x20
#define GYRO_ZOUT_H 0x21
#define GYRO_ZOUT_L 0x22

//This is a list of settings that can be loaded into the registers.
//DLPF, Full Scale Register Bits
//FS_SEL must be set to 3 for proper operation
//Set DLPF_CFG to 3 for 1kHz Fint and 42 Hz Low Pass Filter
#define DLPF_CFG_0 (1<<0)
#define DLPF_CFG_1 (1<<1)
#define DLPF_CFG_2 (1<<2)
#define DLPF_FS_SEL_0 (1<<3)
#define DLPF_FS_SEL_1 (1<<4)

//I2C devices each have an address. The address is defined in the datasheet for the device. The ITG-3200 breakout board can have different address depending on how
//the jumper on top of the board is configured. By default, the jumper is connected to the VDD pin. When the jumper is connected to the VDD pin the I2C address
//is 0x69.
#define itgAddress 0x69
#define toDegrees  1/14.375

//for offsets / calibration

void itgWrite(char address, char registerAddress, char data);
unsigned char itgRead(char address, char registerAddress);

class Gyroscope
{
  
  public:
    Gyroscope();
    int readX();
    int readY();
    int readZ();
    void configureDegree(char data);
    void configureSampleRate(char data);
};


#endif
