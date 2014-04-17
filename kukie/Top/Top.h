//
//  Top.h
//  
//
//  Created by William Ku on 4/17/14.
//
//

#ifndef _Top_h
#define _Top_h

#include "Arduino.h"
#include "Driver.h"
#include "Encoder.h"
#include "IRsensor.h"
#include "PID.h"

// *** Constants ***
// IR (Analog)
const int left = A7;
const int diagleft = A6;
const int frontleft = A3;
const int frontright = A2;
const int diagright = A1;
const int right = A0;
int irThCm = 8;

const float lc1 = 1123.1;
const float lc2 = -1.201;
const float dlc1 = 0.0;
const float dlc2 = 0.0;
const float flc1 = 1093.8;
const float flc2 = -1.257;
const float frc1 = 1010.0;
const float frc2 = -1.168;
const float drc1 = 0.0;
const float drc2 = 0.0;
const float rc1 = 897.89;
const float rc2 = -1.178;

// Speed (Analog)
const int dwheelmm = 32;
const int leftwheel = 0; // D2 pin (interrupt 0)
const int rightwheel = 1; // D3 pin (interrupt 1)

// Drive (Digital)
const int ain1 =  7;
const int ain2 =  4;
const int pwma =  5;
const int bin1 =  8;
const int bin2 =  12;
const int pwmb =  6;

// PID
const float kp = 0.3;
const float kd = 0.1;

// *** Variables ***
volatile unsigned long edge = 0;

#endif
