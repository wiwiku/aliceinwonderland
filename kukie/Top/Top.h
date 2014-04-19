//
//  Top.h
//
//  This class has all the constants such as:
//  (1) Pin mapping
//  (2) Experiment-determined constants
//  (3) volatile variables that are used globally
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
int irThSide = 15;
int irThDiag = 15;
int irThFront = 15;

const float lc1 = 981.58;
const float lc2 = -1.07;
const float dlc1 = 1071.6;
const float dlc2 = -1.138;
const float flc1 = 1186.7;
const float flc2 = -1.057;
const float frc1 = 1084;
const float frc2 = -0.95;
const float drc1 = 1163.4;
const float drc2 = -1.112;
const float rc1 = 1092.7;
const float rc2 = -1.175;

// Speed (Analog)
const int dwheelmm = 32;
const int leftwheel = 0; // D2 pin (interrupt 0)
const int rightwheel = 1; // D3 pin (interrupt 1)

const int edgePerSq = 51;

// Drive (Digital)
const int ain1 =  7;
const int ain2 =  4;
const int pwma =  5;
const int bin1 =  8;
const int bin2 =  12;
const int pwmb =  6;

const int forwardSpeed = 10;
const int backwardSpeed = 10;

// PID
const float kp = 0.5;
const float kd = 0.0;
// 0.3 0.1

// *** Variables ***
volatile unsigned long ledge = 0;
volatile unsigned long redge = 0;


#endif
